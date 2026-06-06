#pragma once
#ifndef VGA_H
#define VGA_H
#include <stdarg.h>
#include <stdint.h>
// Farewell, VGA text mode support
//void set_cursor_pos(int row, int col);
//void set_color(unsigned char vga_color);
//void clear_screen_vga();
void set_color(uint32_t bg, uint32_t fg);
void set_ftimestamp(double timestamp, char *buf);
int putc(int c);
int puts(const char *s);
int print(const char *s, int length); // this can work, but i wouldn't use it if i were you
int printk(unsigned int pass_loglevel, const char *str, ...);
int printf(const char *restrict str, ...);
int cprintk(unsigned int pass_loglevel, const char *str, va_list params);
void draw_rect(int x, int y, int w, int h, uint32_t color);
#endif
