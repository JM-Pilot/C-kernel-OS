#include <port.h>
#include <stdint.h>
#include <generated/config.h>

void set_pcspkr_frequency(int frequency) {
#ifdef CONFIG_PCSPK
#if CONFIG_PCSPK
	outb(0x43, 0xB6);
	uint16_t div = 1193182/frequency;
	outb(0x42, div & 0xFF);
	outb(0x42, div >> 8);
	uint8_t tmp = inb(0x61);
	if ((tmp & 3) != 3) outb(0x61, tmp | 3);
#else
	(void)frequency;
#endif
#endif
}
void no_pcspkr() {
#ifdef CONFIG_PCSPK
#if CONFIG_PCSPK
	uint8_t tmp = inb(0x61);
	outb(0x61, tmp & ~3);
#endif
#endif
}
