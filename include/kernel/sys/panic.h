#pragma once
#ifndef PANIC_H
#define PANIC_H
#include <drivers/video/vga.h>
#include <stddef.h>
#include <arch/i386/idt.h>
extern regs_t *regs;
extern int regs_available;
void oops(const char *msg, ...);
void panic(const char *msg, ...);
#endif
