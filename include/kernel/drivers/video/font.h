#ifndef FONT_H
#define FONT_H
#include <stdint.h>
#define PSF1_MAGIC 0x0436
typedef struct {
	uint16_t magic;
	uint8_t mode;
	uint8_t char_size;
} psf1_head_t;
void font_init();
void put_char(char c);
void clear_screen();
void flush_term();
#endif
