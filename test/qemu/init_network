#!/bin/sh

export PATH=/bin:/sbin:/usr/bin:/usr/sbin

/bin/busybox --install -s


ip link set lo up
ip link set eth0 up

udhcpc

ifconfig eth0 10.0.2.15 netmask 255.255.255.0

/bin/sh 
