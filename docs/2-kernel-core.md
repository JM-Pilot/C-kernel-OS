# 2 - Kernel Core
The CkOS kernel consists of not much of a core. It's just as simple as it gets.

## Init
The kernel start (entry point) is `_start()` in src/boot.s. It sets up the stack and calls `kmain()` from src/entry.c, and if the `kmain()` function somehow returns, `panic()` from include/panic.h is called.

### Stages
- `serial_init()`
- `parse_cmdline(cmdline)`
- `fb_init(fb_info)`
- memory map parsing and display
- `kb_init()`
- `gdt_init()`
- `pic_remap()`
- `init_idt()`
- `__asm__ volatile ("sti")`
- `init_pit()`

## How does the kernel embed a build date?
The Makefile runs a few scripts on build every time it is run.

The script in question is called gen\_ver.sh. Its job is to fetch the user, hostname, run date and operating system type.

The script manually generates a header file for C, or in our case, the C kernel.

This is almost exactly what Linux already does. You can see the header saying similar stuff (except the compiler in the provided script).

Then, the .config file is parsed and converted. The result is a long C header defining all the macros for config. The code looks ugly, because it's generated.

## Drivers
(See ./1-drivers.md)
