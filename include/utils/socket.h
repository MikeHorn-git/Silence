#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

typedef unsigned char Byte;

struct socket *socket_create(void);
int socket_connect(struct socket *sock, const char *ip, unsigned int port);
int socket_send(struct socket *sock, const char *send_buf, unsigned int size);
char *socket_recv(struct socket *sock, unsigned int size);
void socket_close(struct socket *sock);

#endif
