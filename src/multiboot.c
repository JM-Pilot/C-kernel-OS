#include <multiboot.h>
#include <vga.h>
#include <stdint.h>
#include <globals.h>

static fb_info_t framebuffer_info_real;
fb_info_t *framebuffer_info = &framebuffer_info_real;

static inline void _set_pixel(fb_info_t *fb_info, int x, int y, uint32_t color) {
	uint32_t *row = (uint32_t*)((uint8_t*)fb_info->fb+y*fb_info->pitch);
	row[x] = color & 0x00FFFFFF;
}

void set_pixel(int x, int y, unsigned int color) {
	_set_pixel(framebuffer_info, x, y, color);
}

void clear_screen(fb_info_t *fb_info) {
	for (unsigned int i = 0; i < fb_info->w; i++) {
		for (unsigned int j = 0; j < fb_info->h; j++) {
			set_pixel(i, j, 0x00000000);
		}
	}
}

// this demo is garbage, don't use it :)

/*void fb_demo_1(fb_info_t *fbi) {
	uint32_t *framebuffer = (uint32_t*)(uintptr_t)fbi->fb;
	printk(6, "Beginning filling the screen with gray...");
	for (unsigned int i = 0; i < fbi->w*fbi->h*(fbi->bpp/8); i++) {
		framebuffer[i] = 0x00808080;
	}
	printk(6, "Done!");
}*/

void fb_demo_2(fb_info_t *fbi) {
	printk(4, "Press 'q' to exit");
	printk(6, "Drawing pattern...");
	for (int o = 0; o < 1000000; o++) {
		for (unsigned int i = 0; i < fbi->w; i++) {
			for (unsigned int j = 0; j < fbi->h; j++) {
				set_pixel(i, j, (0x00000A00 | ((i-j+(o*15))+5 % 150) << (o % 17)));
				if (kbc) {
					if (kbc == 'q') {
						kbc = 0;
						clear_screen(fbi);
						printk(6, "User-requested exit.");
						return;
					} else {
						kbc = 0;
					}
				}
			}
		}
	}
	printk(6, "Done!");
}

void fb_init(fb_info_t *fbi) {
	framebuffer_info = fbi;
	uint32_t *framebuffer = 0;
	(void)framebuffer;
	printk(6, "Framebuffer is at %x", fbi->fb);
	printk(6, "Size: %dx%dx%d", fbi->w, fbi->h, fbi->bpp); //mbi->fb_width, mbi->fb_height, mbi->fb_bpp);
	printk(6, "Multiboot flags: %x", fbi->flags);
	if (fbi->flags & (1 << 12) && fbi->type == 1) {
		framebuffer = fbi->fb; //(uint8_t*)(uintptr_t)fb;
		printk(4, "Framebuffer info found!");
		//fb_demo_2(fbi);
	} else {
		printk(2, "Framebuffer info unavailable!");
	}
	return;
}

