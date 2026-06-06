#ifndef PORT_H
#define PORT_H
#include <stdint.h>

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);
void wait_port();
void set_post(uint8_t value);
static inline void sti() {
	__asm__ volatile ("sti");
}
static inline void cli() {
	__asm__ volatile ("cli");
}
#endif
