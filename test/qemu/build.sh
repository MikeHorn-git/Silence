#!/bin/bash
# Build your kernel before running this script

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_bzImage>"
    exit 1
fi

KERNEL_PATH="$1"

DISK_IMG="disk.img"
DISK_QCOW2="disk.qcow2"
DISK_SIZE="450M"
LOOP_BACK_DEV=""
ROOTFS_DIR="/tmp/my-rootfs"
SHARE_DIR="/tmp/qemu-share"

# Check if disk image exists
if [ ! -f "$DISK_IMG" ]; then
    echo "Creating disk image..."
    truncate -s $DISK_SIZE $DISK_IMG

    echo "Creating partition table..."
    /sbin/parted -s $DISK_IMG mktable msdos
    /sbin/parted -s $DISK_IMG mkpart primary ext4 1 "100%"
    /sbin/parted -s $DISK_IMG set 1 boot on
else
    echo "Disk image '$DISK_IMG' already exists. Skipping disk creation steps."
fi

# Set up loop device if not already set
echo "Set up loop device..."
sudo losetup -Pf "$DISK_IMG"
LOOP_BACK_DEV=$(losetup -j "$DISK_IMG" | awk -F: '{print $1}' | head -n 1)
if [ -z "$LOOP_BACK_DEV" ]; then
    echo "Error: Failed to set up loop device."
    exit 1
fi
echo "Loop device set up at $LOOP_BACK_DEV..."

echo "Formatting partition as ext4..."
sudo mkfs.ext4 "${LOOP_BACK_DEV}p1"

# Mount partition if not already mounted
if ! mountpoint -q $ROOTFS_DIR; then
    echo "Mounting partition..."
    mkdir -p "$ROOTFS_DIR"
    sudo mount "${LOOP_BACK_DEV}p1" "$ROOTFS_DIR"
else
    echo "Partition already mounted at '$ROOTFS_DIR'."
fi

# Check if root filesystem is populated
if [ ! -d "$ROOTFS_DIR/bin" ]; then
    echo "Installing minimal Alpine Linux..."
    sudo docker run -it --rm -v $ROOTFS_DIR:/my-rootfs alpine sh -c '
      apk add build-base iputils-ping linux-headers openrc rsyslog sudo util-linux;
      service rsyslog start
      rc-update add rsyslog default
      ln -s agetty /etc/init.d/agetty.ttyS0;
      echo ttyS0 > /etc/securetty;
      rc-update add agetty.ttyS0 default;
      rc-update add root default;
      echo "root:password" | chpasswd;
      adduser --disabled-password alpine
      echo "alpine:password" | chpasswd;
      echo "%wheel ALL=(ALL) ALL" > /etc/sudoers.d/wheel
      adduser alpine wheel
      rc-update add devfs boot;
      rc-update add procfs boot;
      rc-update add sysfs boot;
      for d in bin etc lib root sbin usr; do tar c "/$d" | tar x -C /my-rootfs; done;
      for dir in dev proc run sys var; do mkdir /my-rootfs/${dir}; done;
    '
else
    echo "Root filesystem already populated. Skipping Alpine Linux installation."
fi

echo "Copy shell scripts..."
sudo cp mount.sh network.sh "$ROOTFS_DIR"

echo "Installing GRUB and Kernel..."
sudo mkdir -p $ROOTFS_DIR/boot/grub
sudo cp "$KERNEL_PATH" "$ROOTFS_DIR"/boot/vmlinuz

cat <<EOF | sudo tee $ROOTFS_DIR/boot/grub/grub.cfg
serial
terminal_input serial
terminal_output serial
set root=(hd0,1)
menuentry "Linux2600" {
    linux /boot/vmlinuz root=/dev/sda1 rw console=tty1
}
EOF

sudo grub-install --directory=/usr/lib/grub/i386-pc --boot-directory=$ROOTFS_DIR/boot "$LOOP_BACK_DEV" 
echo "Create shared folder in $SHARE_DIR"
mkdir -p $SHARE_DIR

echo "Convert from RAW to QCOW2..."
qemu-img convert -O qcow2 "$DISK_IMG" "$DISK_QCOW2"
rm "$DISK_IMG"

echo "Cleaning up..."
sudo umount "$ROOTFS_DIR"
sudo losetup -d "$LOOP_BACK_DEV"
