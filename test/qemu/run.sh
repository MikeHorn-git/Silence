#!/bin/sh

SHARE_DIR="/tmp/qemu-share"
# In Qemu VM:
# mkdir -p /tmp/share
# mount -t 9p -o trans=virtio host0 /tmp/share -oversion=9p2000.L

qemu-system-x86_64 \
  -no-reboot \
  -hda disk.qcow2 \
  -nic user,model=virtio-net-pci \
  -nographic \
  -virtfs local,path="$SHARE_DIR",mount_tag=host0,security_model=passthrough,id=foobar
