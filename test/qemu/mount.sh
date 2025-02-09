#!/bin/sh

mkdir -p /tmp/share
mount -t 9p -o trans=virtio host0 /tmp/share -oversion=9p2000.L
