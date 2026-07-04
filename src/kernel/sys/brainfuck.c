#include <sys/globals.h>
#include <drivers/video/vga.h>
#include <stdint.h>
#include <string.h>

uint8_t brainfuck_memory[4096] = {0};

void brainfuck_interpret(char *code) {
	char *pc = code;
	unsigned char *memptr = brainfuck_memory;
	unsigned char *memmin = brainfuck_memory;
	unsigned char *memmax = brainfuck_memory+4095;
	memset(brainfuck_memory, 0, 4096);
	while (*pc) {
		switch (*pc) {
			case '>':
				if (memptr < memmax) memptr++;
				else memptr = memmin;
				break;
			case '<':
				if (memptr > memmin) memptr--;
				else memptr = memmax;
				break;
			case '+':
				(*memptr)++;
				break;
			case '-':
				(*memptr)--;
				break;
			case ',':
				while (!kbc) __asm__ volatile ("pause"); *memptr = kbc; kbc = 0;
				break;
			case '.':
				putc(*memptr);
				break;
			case '[':
				if (!*memptr) {
					int d = 1;
					while (d) {
						pc++;
						if (!*pc) return;
						if (*pc == '[') d++;
						else if (*pc == ']') d--;
					}
				}
				break;
			case ']':
				if (*memptr) {
					int d = 1;
					while (d) {
						pc--;
						if (pc < code) return;
						if (*pc == ']') d++;
						else if (*pc == '[') d--;
					}
				}
				break;
			default:
				break;
		}
		pc++;
	}
}
