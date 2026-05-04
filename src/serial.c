#include <port.h>
#include <stddef.h>
#include <stdint.h>
#include <serial.h>
#include <globals.h>
#include <stdlib.h>
#include <vga.h>
unsigned short COM1 = 0x03F8;
unsigned short COM2 = 0x02F8;
unsigned short UART1 = 0;
unsigned short UART2 = 0;

void __init_com_(unsigned short port) {
	outb(port+1, 0x00);
	outb(port+3, 0x80);
	outb(port+0, 0x01);
	outb(port+1, 0x00); // 0x00
	outb(port+3, 0x03);
	outb(port+1, 0x01);
	outb(port+2, 0xC7); // 0xC7
	outb(port+4, 0x0B);
}

void serial_init() {
	if (*(unsigned short *)0x400) {
		COM1 = *(unsigned short *)0x400;
	}
	if (*(unsigned short *)0x402) {
		COM2 = *(unsigned short *)0x402;
	}
	UART1 = COM1;
	UART2 = COM2;
	__init_com_(COM1);
	printk(5, "serial: Initialized COM1");
	__init_com_(COM2);
	printk(5, "serial: Initialized COM2");
}

void serial_shutdown() {
	serial_out = false;
	serial_in = false;
	outb(COM1+1, 0x00);
	outb(COM2+1, 0x00);
	printk(5, "serial: Serial shut down");
}

int transmit_fifo_empty_com1() {
	return inb(COM1+5) & 0x20;
}

int transmit_fifo_empty_com2() {
	return inb(COM2+5) & 0x20;
}

void sputc(char c) {
	while (!transmit_fifo_empty_com1());
	outb(COM1, c);
	while (!transmit_fifo_empty_com2());
	outb(COM2, c);
}

// same story as puts, but for serial
void sputs(const char *s) {
	for (size_t i = 0; s[i] != '\0'; i++) {
		sputc(s[i]);
	}
}

/*char sgetc_raw() {
	//if (inb(COM1+5) & 1) { return inb(COM1); }
	char c = 0;
	uint8_t state = inb(COM1+5);
	if (!(state & 1)) return 0;
	c = inb(COM1);
	if (state & 0x1E) return 0;
	return c;
}*/

int sgetc_raw(unsigned char port) {
	unsigned short com = port == 1 ? 0x2F8 : 0x3F8;
	uint8_t state = inb(com+5);
	if (!(state & 1)) return -1;
	if (state & 0x1E) {
		inb(com);
		return -1;
	}
	unsigned char c = inb(com);
	if (!c) return 0;
	return c;
}

char sgetc(unsigned char port) {
	char c = sgetc_raw(port);
	if (c < 0) return 0;
	if (c == '\r') {
		return '\n';
	} else if (c == 0x7F) {
		sputc('\b');
		sputc(' ');
		//sputc('\b');
		return '\b';
	}
	return c;
}
