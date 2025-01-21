#include "socket.h"

#include <linux/in.h>
#include <linux/inet.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/slab.h>
#include <linux/socket.h>
#include <linux/uio.h>
#include <net/inet_common.h>
#include <net/sock.h>

struct socket *socket_create(void)
{
	struct socket *sock = NULL;
	int ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP,
				   &sock);
	if (ret < 0) {
		return NULL;
	}
	return sock;
}

int socket_connect(struct socket *sock, const char *ip, unsigned int port)
{
	struct sockaddr_in s_addr = { 0 };
	int ret;

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port);
	if (in4_pton(ip, -1, (u8 *)&s_addr.sin_addr.s_addr, '\0', NULL) == 0) {
		return -EINVAL;
	}

	ret = sock->ops->connect(sock, (struct sockaddr *)&s_addr,
				 sizeof(s_addr), 0);
	if (ret) {
	}
	return ret;
}

int socket_send(struct socket *sock, const char *send_buf, unsigned int size)
{
	struct kvec send_vec;
	struct msghdr send_msg = { 0 };
	int ret;

	send_vec.iov_base = (void *)send_buf;
	send_vec.iov_len = size;

	ret = kernel_sendmsg(sock, &send_msg, &send_vec, 1, size);
	if (ret < 0) {
	}
	return ret;
}

char *socket_recv(struct socket *sock, unsigned int size)
{
	struct kvec recv_vec;
	struct msghdr recv_msg = { 0 };
	char *recv_buf;
	int ret;

	recv_buf = kmalloc(size, GFP_KERNEL);
	if (!recv_buf) {
		return NULL;
	}

	recv_vec.iov_base = recv_buf;
	recv_vec.iov_len = size;

	ret = kernel_recvmsg(sock, &recv_msg, &recv_vec, 1, size, 0);
	if (ret < 0) {
		kfree(recv_buf);
		return NULL;
	}

	return recv_buf;
}

void socket_close(struct socket *sock)
{
	if (sock) {
		kernel_sock_shutdown(sock, SHUT_RDWR);
		sock_release(sock);
	}
}

EXPORT_SYMBOL(socket_create);
EXPORT_SYMBOL(socket_connect);
EXPORT_SYMBOL(socket_send);
EXPORT_SYMBOL(socket_recv);
EXPORT_SYMBOL(socket_close);
