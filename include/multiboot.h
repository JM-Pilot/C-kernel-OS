#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include <stdint.h>
typedef struct multiboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mod_count;
	uint32_t mod_addr;
	uint32_t syms[4];
	uint32_t memmap_length;
	uint32_t *memmap_addr;
	uint32_t drive_length;
	uint32_t drive_addr;
	uint32_t cfg_table;
	uint32_t bootloader;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
	uint64_t fb_addr;
	uint32_t fb_pitch;
	uint32_t fb_width;
	uint32_t fb_height;
	uint8_t fb_bpp;
	uint8_t fb_type;
	union {
		struct {
			uint32_t fb_palette_addr;
			uint16_t fb_palette_color_n;
		};
		struct {
			uint8_t fb_rpos;
			uint8_t fb_rsize;
			uint8_t fb_gpos;
			uint8_t fb_gsize;
			uint8_t fb_bpos;
			uint8_t fb_bsize;
			// TODO finish those according to color_info_t
		};
	};
} __attribute__((packed)) mbinfo_t;

typedef struct {
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) mb_memmap_t;

typedef struct {
	uint8_t red_pos;
	uint8_t red_size;
	uint8_t green_pos;
	uint8_t green_size;
	uint8_t blue_pos;
	uint8_t blue_size;
} color_info_t;

typedef struct {
	uint32_t flags;
	uint32_t w;
	uint32_t h;
	uint32_t bpp;
	uint32_t *fb;
	uint8_t type;
	uint32_t pitch;
	color_info_t *color_info;
} fb_info_t;

extern uint8_t *framebuffer;
void fb_init(fb_info_t *fb_info);
void fb_debug_print(fb_info_t *fbi);
void fb_demo_2(fb_info_t *fbi);
void fb_demo_3(fb_info_t *fbi);
void fb_demo_4(fb_info_t *fbi);
void set_pixel(int x, int y, unsigned int color);

extern fb_info_t *framebuffer_info;
#endif
