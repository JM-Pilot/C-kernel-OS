#include <globals.h> // Globals
#include <vga.h> // Display characters and string
#include <kb.h> // Keyboard functions (deprecated outside IDT)
#include <string.h> // String tools
#include <panic.h> // Critical errors
#include <stdlib.h> // Number to string functions
#include <serial.h> // Serial I/O
#include <power.h> // Power options
#include <port.h> // Port I/O
#include <multiboot.h> // Multiboot info for command line
#include <idt.h> // Interrupt handling
#include <gdt.h> // Required for IDT
#include <pit.h> // Programmable Interval Timer
#include <pic.h> // PIC
#include <generated/__GENVER.h> // Build info
#include <cpu.h> // CPU vendor string
#include <generated/__BLD.h> // Build number
#include <stddef.h> // Stuff like uint8_t
#include <font.h> // Framebuffer functions
#include <generated/config.h> // Kernel config (obviously)
#include <pcspkr.h>
#include <math.h>

static fb_info_t fb_info_real;
static color_info_t color_info_real;

fb_info_t *fb_info = &fb_info_real;
color_info_t *color_info = &color_info_real;

char *__split_cmdline(char **buffer) {
	char *a, *b;
	a = *buffer;
	if (!a) return NULL;
	for (b = a; *b && strchr(b, ' '); b++);
	if (*b) {
		*b++ = '\0';
		*buffer = b;
	} else {
		*buffer = NULL;
	}
	return a;
}

void parse_cmdline(char *input) {
	char *a; char *b = input;
	while ((a = __split_cmdline(&b))) {
		if (*a == '\0') continue;
		printk(6, "Received command line argument: %s", a);
		if (strcmp(a, "shut_up") == 0) {
			printk(7, "Shh");
			loglevel = 2;
		} else if (strcmp(a, "s_out") == 0) {
			serial_out = true;
			//sputs("\e[0m\e[H\e[J\e[1J\e[2J\r\e[=7h\e[?25h");
			/*
			 * In order:
			 * '\e[0m'            disable all modes
			 * '\e[H'             move cursor to (0, 0)
			 * '\e[J'             erase what's displayed
			 * '\e[1J'            erase from cursor to beginning of screen
			 * '\e[2J'            erase entire screen
			 * '\e[=7h'           enable line wrap
			 * '\e[?25h'          make cursor visible
			 * Thanks to @fnky on GitHub for their Gist on this!
			 */
			printk(6, "Serial output enabled");
		} else if (strcmp(a, "s_in") == 0) {
			serial_in = true;
			printk(6, "Serial input enabled");
		} else {
			printk(2, "Invalid command line argument %s, ignoring", a);
		}
	}
}

extern void _start();
void debug_info_print() {
	printk(6, ver); printk(6, "Build #%d by %s", BUILD_NUMBER, CKOS_BLD);
	printk(6, "Git source tree commit hash: %s", GIT_SOURCE_HASH);
	printk(6, "Kernel entry offset: %x, image offset: %x", _start, 1024*1024);
	printk(7, "Made by orca.pet3910YT with %s", "\x03");
}

void vga_old_putc(char c, short loc) {
	((uint16_t*)0xB8000)[loc] = 0x0700 | c;
}

struct cpufreq_s *cpufreq;
char *no_fb_err = "A framebuffer is required to continue. If you have serial, boot logs will go there.";
char command[256] = {0};

void kmain(int magic, uint32_t *mbi) {
	(void)magic;
	//volatile char* video = (volatile char*)0xB8000;
	//video[0] = 'E';
	//video[1] = 0x07;
	//putc('h');
	//putc('\n');
	//putc('i');
	//char string_thing[12] = {0};
	//ftoa(0.25, string_thing);
	//puts(string_thing);
	/*
	 * do not even attempt to remove the comments above.
	 */
	set_post(0x3E);

	/*fb_info->flags = mbi->flags;
	fb_info->w = mbi->fb_width;
	fb_info->h = mbi->fb_height;
	fb_info->bpp = mbi->fb_bpp;
	fb_info->fb = (uint32_t*)(uintptr_t)mbi->fb_addr;
	fb_info->type = mbi->fb_type;
	fb_info->pitch = mbi->fb_pitch;
	color_info->red_pos = mbi->fb_rpos;
	color_info->red_size = mbi->fb_rsize;
	color_info->green_pos = mbi->fb_gpos;
	color_info->green_size = mbi->fb_gsize;
	color_info->blue_pos = mbi->fb_bpos;
	color_info->blue_size = mbi->fb_bsize;
	fb_info->color_info = color_info;*/
	uint32_t *mbi_old = mbi;
	serial_init();
	char *cmdline = NULL;
	char can_font_init = 0;
	uint8_t *ptr = (uint8_t*)mbi;
	uint32_t mbi_size = *mbi;
	ptr += 8;
	while (ptr < (uint8_t*)mbi+mbi_size) {
		printk(7, "tag type %d size %d ptr %x", *(uint32_t*)ptr, *(uint32_t*)(ptr+4), ptr);
		if (!*(uint32_t*)ptr) break;
		if (*(uint32_t*)ptr == 8) {
			fb_info->fb = (uint32_t*)(uintptr_t)(*(uint64_t*)(ptr+8));
			fb_info->pitch = *(uint32_t*)(ptr+16);
			fb_info->w = *(uint32_t*)(ptr+20);
			fb_info->h = *(uint32_t*)(ptr+24);
			fb_info->bpp = *(uint8_t*)(ptr+28);
			if (fb_info->bpp == 32) can_font_init = 1;
			fb_init(fb_info, can_font_init);
		} else if (*(uint32_t*)ptr == 1) {
#ifdef CONFIG_CMDLINE_STR
#if CONFIG_CMDLINE
			cmdline = CONFIG_CMDLINE_STR;
#endif
#else
#if !CONFIG_CMDLINE
			cmdline = (char*)(ptr+8);
#else
#error "possible config corruption"
#endif
#endif
		} else if (*(uint32_t*)ptr == 2) {
			printk(6, "Booting via %s", (char*)(ptr+8));
		} else if (*(uint32_t*)ptr == 14) {
			unsigned char rsdp_sign[9] = {0};
			memcpy(rsdp_sign, ptr+8, 8);
			printk(6, "RSDP signature: '%s', revision %d, RSDP at %x", &rsdp_sign, (uint32_t)*(ptr+23), *(uint32_t*)(ptr+24));
		}
		ptr += (*(uint32_t*)(ptr+4)+7) & ~7;
	}
	//printk(6, "---BEGIN Command line info---");
	parse_cmdline(cmdline);
	//printk(4, "Parsed command line provided by bootloader");
	//printk(6, "--- END Command line info ---");
	printk(0, "Hello, hello!");
	printk(0, "%x %x %x %x", (uint32_t)mbi_old, (uint32_t)mbi, (uint32_t)ptr, (uint32_t)can_font_init);
	/*if (!(mbi->flags & (1<<12))) {
		for (int i = 0; *no_fb_err; i++) {
			vga_old_putc(*no_fb_err++, i);
		}
		printk(3, "No VBE framebuffer is usable.");
	} else {
		fb_init(fb_info);
	}*/
	debug_info_print();
	printk(5, "Command line: %s", cmdline);
	printk(6, "Initialized serial at %x (COM1) and %x (COM2)", UART1, UART2);
	// FIXME: magic is 0
	//if (magic != 0x1BADB002) panic("Incorrect Multiboot 1 magic number! Got 0x%x, should be 0x1BADB002", magic);
	if (serial_out) {
		printk(4, "Printing framebuffer info for serial console");
		fb_debug_print(fb_info);
	}
	/*uint8_t *mmap = (uint8_t*)mbi->memmap_addr;
	uint8_t *mmap_end = mmap + mbi->memmap_length;
	while (mmap < mmap_end) {
		mb_memmap_t *entry = (mb_memmap_t*)mmap;
		uint32_t base = entry->addr;
		uint64_t len = entry->len;
		uint32_t end = base+len;
		uint32_t type = entry->type;
		char *user_type = "";
		if (type == 1) user_type = "Usable";
		if (type == 2) user_type = "Reserved";
		if (type == 3) user_type = "ACPI Data";
		if (type == 4) user_type = "ACPI NV Storage";
		if (type == 5) user_type = "Bad RAM";
		if (!type || type > 6) user_type = "Unknown";
		printk(4, "Memory [%x-%x] is of type %s", base, end, user_type);
		mmap += entry->size + sizeof(entry->size);
	}*/
	kb_init();
	printk(6, "Initialized PS/2 BIOS keyboard");
	gdt_init();
	printk(6, "GDT initialized");
	pic_remap();
	printk(6, "Remapped the PIC");
	init_idt();
	printk(6, "IDT initialized");
	init_pit();
	printk(6, "Initialized PIT at 10 KHz");
	__asm__ volatile ("sti");
	printk(6, "Set interrupts");
	printk(7, "isr6(): %x", isr6);
	printk(7, "isr0(): %x", isr0);
	printk(7, "CPU: %s", get_cpu_vendor());
	// get_cpu_vendor in cpu.asm is a simple assembly function that uses cpuid with eax = 0
	// which puts the max eax value in eax and the CPU vendor in ebx, edx and ecx in order.
	// technically i don't think you have to call it more than once but never too safe :P
	char brand[13];
	printk(7, "---> %s", get_cpu_brand(brand));
	volatile uint32_t pre_test_time = uptime_ticks;
	for (int i = 0; i < 10000000; i++) __asm__ volatile ("data16 ds cs nopl 0x0(%eax, %eax, 1)");
	volatile uint32_t post_test_time = uptime_ticks;
	printk(4, "CPU test passed, took %d ms for a 10 million NOPL repetitions loop", (post_test_time-pre_test_time)/10);
	uint32_t __uptimeticks = (uint32_t)uptime_ticks;
	pre_test_time = uptime_ticks;
	for (int i = 0; i < 10000000; i++) {
		__uptimeticks = (uint32_t)uptime_ticks;
		__asm__ volatile ("xorl %[pattern], %[ticks]" : [ticks] "+r" (__uptimeticks) : [pattern] "r" (i) : "cc");
	}
	post_test_time = uptime_ticks;
	printk(4, "CPU test passed, took %d ms for a 10 million XOR repetitions loop", (post_test_time-pre_test_time)/10);
	printk(4, "square root of 4 is %d and of 9 is %d", sqrt(4), sqrt(9));
	printk(4, "4 is%s a prime, 0x7FFFFFFF is%s a prime and 13 is%s a prime", is_prime(4) ? "" : " not", is_prime(0x7FFFFFFF) ? "" : " not", is_prime(13) ? "" : " not");
	printk(7, "Hello, World!");
#ifdef CONFIG_LOGO
#if CONFIG_LOGO
	uint32_t color = fg_color;
	set_color(bg_color, 0x00FFFFFF);
	printf("%s\n", logo); // globals.h:4
	set_color(bg_color, color);
#endif
#endif
	printk(0, "Welcome to CkOS!");
	set_post(0x00);
	//panic("This PC is ass."); // compile with this uncommented to prank people :)
	int index = 0;
	printf("$ ");
	for (;;) {
		char c = kbc; // from globals
		__asm__ volatile ("pause");
		if (c) { if (c != '\b') { putc(c); } kbc = 0; } else { __asm__ volatile ("hlt"); continue; }
		if (c == '\b') {
			if (index > 0) {
				index--;
				command[index] = '\0';
				putc('\b');
			}
			flush_term();
			//lastchar--;
			continue;
		}
		if (c == 0x1A || c == 0x1B || c == 0x1E || c == 0x1F) { // all of these are arrow keys
			continue; // arrow handling is painful in our world
		}
		if (c != '\n' && index < 256) {
			command[index++] = c;
			command[index] = '\0';
		}
		flush_term();
		if (c == '\n') {
			if (strncmp(command, "help", 4) == 0) {
				printf("The commands are:\n"
				"hello: say hello to the world\n"
				"poweroff: turn the system off (QEMU only)\n"
				"reboot: restart the system\n"
				"halt: halt the CPU putting the entire system to a freeze\n"
				"help: display this message\n"
				"logo: display the logo\n"
				"ver: display the version\n"
				"clear: clear the screen\n"
				"panictest: test the panic functionality\n"
				"crash: triggers a crash\n"
				"credits: show credits\n"
				"cpuinfo: get CPU info\n"
				"delaytest: test delay functions\n"
				"fb_demo: framebuffer demo\n"
				"uptime: get OS uptime\n"
				"echo: output a string\n"
				"beep: make a 250 ms 440 Hz beep\n");
			} else if (strncmp(command, "hello", 5) == 0) {
				printf("Hello, World!\n");
			} else if (strncmp(command, "poweroff", 8) == 0) {
				poweroff();
				panic("Failed to power off; likely not a QEMU machine");
			} else if (strncmp(command, "reboot", 6) == 0) {
				reboot();
				__asm__ volatile ("hlt");
				panic("Failed to reboot; unknown error");
			} else if (strncmp(command, "halt", 4) == 0) {
				printf("System halted. It is now safe to power off.\n");
				while (1) halt();
			} else if (strncmp(command, "logo", 4) == 0) {
				set_color(0x00000000, 0x00FFFFFF);
				printf("%s\n", logo);
				set_color(0x00000000, 0x00AAAAAA);
			} else if (strncmp(command, "ver", 3) == 0) {
				printf("%s\n", ver);
			} else if (strncmp(command, "panictest", 9) == 0) {
				panic("User-triggered panic");
			} else if (strncmp(command, "clear", 5) == 0) {
				clear_screen();
			} else if (strncmp(command, "credits", 7) == 0) {
				printf(credits);
			} else if (strncmp(command, "crash", 5) == 0) {
				__asm__ volatile ("int3");
			} else if (strncmp(command, "cpuinfo", 7) == 0) {
				get_cpu_brand(brand);
				printf("CPU vendor: '%s', friendly name '%s'\n", get_cpu_vendor(), get_cpu_vendor_user());
				printf("CPU brand: '%s'\n", brand);
				struct cpufreq_s _temp = get_cpu_clk();
				*cpufreq = _temp;
				printf("CPU clock speeds (CPUID EAX=16h):\nBase: %d MHz, max: %d MHz, Bus: %d MHz\n", cpufreq->base, cpufreq->max, cpufreq->bus);
				// FIXME
				//unsigned int clk_d = get_cpu_clk_d();
				//printf("TSC-based clock speed: %d\n", clk_d);
			} else if (strncmp(command, "oopstest", 8) == 0) {
				oops("User-triggered oops");
			} else if (strncmp(command, "delaytest", 9) == 0) {
				uint32_t ms_elapsed[3] = {0};
				uint32_t time = uptime_ticks/10;
				delay(500);
				ms_elapsed[0] = uptime_ticks/10-time;
				time = uptime_ticks/10;
				delay(1000);
				ms_elapsed[1] = uptime_ticks/10-time;
				time = uptime_ticks/10;
				delay(5000);
				ms_elapsed[2] = uptime_ticks/10-time;
				printf("done, all tests passed!\n");
				printf("test 0 (500 ms): %d\ntest 1 (1 sec): %d\ntest 2 (5 sec): %d\n", ms_elapsed[0], ms_elapsed[1], ms_elapsed[2]);
			} else if (strncmp(command, "fb_demo", 7) == 0) {
#ifdef CONFIG_ANIMATIONS
#if CONFIG_ANIMATIONS
				if (!command[8]) printf("Please pick a demo by passing a digit as a command argument. Available demos: 0, 1, 2\n");
				else if (command[8] == '0') fb_demo_2(fb_info);
				else if (command[8] == '1') fb_demo_3(fb_info);
				else if (command[8] == '2') fb_demo_4(fb_info);
				else printf("Invalid demo\n");
#else
				printf("Animations are disabled, please recompile if that's a mistake\n");
#endif
#endif /* CONFIG_ANIMATIONS */
			} else if (strncmp(command, "echo", 4) == 0) {
				puts(command+5);
				putc('\n');
			} else if (strncmp(command, "uptime", 6) == 0) {
				uint32_t uptime_secs = (uint32_t)(uptime_ticks)/10000; // src/pit.c
				printf("Uptime in seconds: %d\n", uptime_secs);
				uint32_t uptime_minutes = uptime_secs/60;
				uint32_t uptime_hours = uptime_minutes/60;
				uint32_t uptime_days = uptime_hours/24;
				uint32_t uptime_years = uptime_days/365;
				if (uptime_years == 1) {
					printf("1 year, ");
				} else if (uptime_years > 1) {
					printf("%d years, ", uptime_years);
				}
				if (uptime_days == 1) {
					printf("1 day, ");
				} else if (uptime_days > 1) {
					printf("%d days, ", uptime_days % 365);
				}
				if (uptime_hours == 1) {
					printf("1 hour, ");
				} else if (uptime_hours > 1) {
					printf("%d hours, ", uptime_hours % 24);
				}
				if (uptime_minutes == 1) {
					printf("1 minute and ");
				} else if (uptime_minutes > 1) {
					printf("%d minutes and ", uptime_minutes % 60);
				}
				printf("%d seconds\n", uptime_secs % 60);
			} else if (strncmp(command, "beep", 4) == 0) {
				set_pcspkr_frequency(440);
				delay(250);
				no_pcspkr();
			} else if (strncmp(command, "square", 6) == 0) {
				clear_screen();
				draw_rect(150, 150, 150, 150, 0x00FFFFFF);
				delay(3000);
				clear_screen();
				draw_rect_outline(100, 100, 125, 125, 0x00FFFFFF, 5);
				delay(3000);
				clear_screen();
			} else if (index > 0) {
				printf("Invalid command: %s\n", command);
			}
			index = 0;
			for (uint32_t i = 0; i < 256; i++) {
				command[i] = '\0';
			}
			printf("$ ");
			continue;
		}
	}
}
