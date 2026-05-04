#include <multiboot.h>
#include <vga.h>
#include <stdint.h>

void fb_init(fb_info_t *fbi) {
	uint8_t *framebuffer = 0;
	printk(6, "Framebuffer is at %x", fbi->fb);
	printk(6, "Size: %dx%dx%d", fbi->w, fbi->h, fbi->bpp); //mbi->fb_width, mbi->fb_height, mbi->fb_bpp);
	printk(6, "Multiboot flags: %x", fbi->flags);
	if (fbi->flags & (1 << 12)) {
		framebuffer = fbi->fb; //(uint8_t*)(uintptr_t)fb;
		printk(4, "Framebuffer info found!");
	} else {
		printk(2, "Framebuffer info unavailable!");
		//framebuffer = (uint8_t*)0xA0000;
	}
	printk(6, "Beginning filling the screen with gray...");
	printk(6, "Done!");
	for (unsigned int i = 0; i < fbi->w*fbi->h*(fbi->bpp/8); i++) {
		framebuffer[i] = 0x80;
	}
	return;
}
