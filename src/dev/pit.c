#include <drivers/timers/pit.h>
#include <drivers/video/vga.h>
#include <arch/i386/port.h>
#include <stdlib.h>
#include <sys/globals.h>

#define MAX_LEN_NOP "\
		data16 data16 data16 data16 \
		data16 data16 data16 data16 \
		data16 nopw %cs:0x0(%eax,%eax,1)"
// just don't use the string above because why would you do that

void init_pit() {
	pit_set_div(10000);
}

void pit_shutdown() {
	outb(0x43, 0x30);
	outb(0x40, 0x00);
	outb(0x40, 0x00);
	outb(0x21, inb(0x21) | 1);
}

void pit_set_div(uint16_t hz) {
	uint16_t div = PIT_BASE/hz;
	outb(0x43, 0x36);
	wait_port();
	outb(0x40, (uint8_t)(div & 0xFF));
	wait_port();
	outb(0x40, (uint8_t)((div >> 8) & 0xFF));
	wait_port();
}

void pit_tick(regs_t *r) {
	(void)r;
	//uptime += 0.0007;
	uptime_ticks++;
}

void retrieve_uptime() {
	uptime = (float)uptime_ticks*0.0001f; // prefer this over increasing the float directly because of precision loss
}

void delay(unsigned int ms) {
	unsigned int begin = uptime_ticks;
	unsigned int ticks = ms*10;
	while ((uptime_ticks-begin) < ticks) __asm__ volatile ("pause"); // pause tells the CPU to not go full speed in the loop which is useful here
}
