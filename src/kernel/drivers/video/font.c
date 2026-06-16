#include <drivers/video/font.h>
#include <stdint.h>
#include <sys/multiboot.h>
#include <string.h>
#include <sys/globals.h>

#define T_CHAR_WIDTH 8
#define T_CHAR_HEIGHT 16

typedef struct {
	uint32_t fg;
	uint32_t bg;
	unsigned char dirty;
	unsigned char c;
} term_cell_t;

static term_cell_t terminal[350*100] = {0};
int t_width = 0;
int t_height = 0;
/*extern int fb_x;*/ int tx = 0;
/*extern int fb_y;*/ int ty = 0;
uint32_t pitch = 0;
uint32_t pitchp = 0;
uint32_t fb_bpp = 0;

extern uint8_t _binary_fonts_default_8x16_psf_start[];
static psf1_head_t *font_head;

void clear_screen(); // required in order for font_init to call the function before its definition to avoid excessive code movement

void font_init() {
	font_head = (psf1_head_t*)_binary_fonts_default_8x16_psf_start;
	if (font_head->magic != PSF1_MAGIC) {
		return;
	}
	t_width = framebuffer_info->w/T_CHAR_WIDTH;
	t_height = framebuffer_info->h/T_CHAR_HEIGHT;
	pitch = framebuffer_info->pitch;
	fb_bpp = framebuffer_info->bpp;
	pitchp = (pitch*8)/fb_bpp;
	clear_screen();
}

void _putc(unsigned char c, int x, int y, uint32_t fg, uint32_t bg) {
	uint8_t *glyph = (uint8_t*)_binary_fonts_default_8x16_psf_start+4+(c*font_head->char_size);
	for (int r = 0; r < font_head->char_size; r++) {
		uint32_t *row = framebuffer_info->fb+(y+r)*pitchp;
		for (int s = 0; s < 8; s++) {
			int p = (glyph[r] >> (7-s)) & 1;
			//set_pixel(x+s, y+r, p ? fg : bg);
			row[x+s] = p ? fg : bg;
		}
	}
}

void redraw_term() {
	int x, y;
	for (y = 0; y < t_height; y++) {
		for (x = 0; x < t_width; x++) {
			term_cell_t c = terminal[y*t_width+x];
			_putc(c.c, x*T_CHAR_WIDTH, y*T_CHAR_HEIGHT, c.fg, c.bg);
		}
	}
}

void clear_screen() {
	memset(framebuffer_info->fb, 0x00, framebuffer_info->h*framebuffer_info->pitch);
	for (int x = 0; x < t_width; x++) {
		for (int y = 0; y < t_height; y++) {
			terminal[y*t_width+x].c = 0x20;
			terminal[y*t_width+x].fg = fg_color;
			terminal[y*t_width+x].bg = bg_color;
		}
	}
	redraw_term(); tx = 0; ty = 0;
}

void flush_term() {
	for (int y = 0; y < t_height; y++) {
		for (int x = 0; x < t_width; x++) {
			term_cell_t *c = &terminal[y*t_width+x];
			if (!c->dirty) continue;
			_putc(c->c, x*T_CHAR_WIDTH, y*T_CHAR_HEIGHT, c->fg,c->bg);
			c->dirty = 0;
		}
	}
}

void scroll_term() {
	memmove(terminal, terminal+ t_width, sizeof(term_cell_t)*t_width*(t_height-1));
	for (int x = 0; x < t_width; x++) {
		term_cell_t *c = &terminal[(t_height-1)*t_width+x];
		c->c = ' '; c->fg = fg_color; c->bg = bg_color;
	}
	for (int i = 0; i < t_width*t_height; i++) terminal[i].dirty = 1; //(terminal[i].c != ' ');
	ty = t_height - 1; tx = 0;
}

void put_char(char c) {
	if (c == '\n') {
		tx = 0; ty++;
		if (ty >= t_height) scroll_term();
		return;
	}
	if (c == '\b' && tx > 0) {
		tx--;
		terminal[ty*t_width+tx].fg = fg_color;
		terminal[ty*t_width+tx].bg = bg_color;
		terminal[ty*t_width+tx].dirty = terminal[ty*t_width+tx].c == ' ' ? 0 : 1; //terminal[ty*t_width+tx].dirty;
		terminal[ty*t_width+tx].c = ' ';
		return;
	}
	if (ty >= t_height) scroll_term();
	term_cell_t *cell = &terminal[ty*t_width+tx];
	cell->c = c;
	cell->fg = fg_color;
	cell->bg = bg_color;
	cell->dirty = 1; tx++;
	if (tx >= t_width) {
		tx = 0; ty++;
	}
}
