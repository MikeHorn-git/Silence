#! /bin/bash
KERN_VERSION=linux-6.11

wget https://cdn.kernel.org/pub/linux/kernel/v6.x/$KERN_VERSION.tar.xz
tar -xvf "$KERN_VERSION".tar.xz
rm "$KERN_VERSION".tar.xz
cd "$KERN_VERSION" || exit
make defconfig
