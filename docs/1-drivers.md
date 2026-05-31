# 1 - Drivers
There isn't much to this OS and drivers. After all this is below 2k lines.

But the driver list is as follows:
- kb: Keyboard
- pit: PIT
- vga: Graphics
- power: Power modes
- serial: Serial I/O
- port: Raw PMIO
- cpu: CPU info getters
- pcspkr: The PC speaker (buzzer)

## Keyboard
<details>
<summary>Click for details</summary>

The keyboard driver is crazy. This is for PS/2, but CSM turns a USB keyboard to PS/2 I/O.

You initialize this driver by calling `kb_init()`. I doubt this does much because I forgot
to initialize it in really early 0.01.

You have to get the raw scancodes by checking if `inb(0x64) & 1` is truthy.
Then, you read a byte from `inb(0x60)`. Even then, you only get scancodes.

It is up to you how to handle the scancode in the kernel. It gives you data on when the key is pressed
or released.
</details>

## PIT
<details>
<summary>Click for details</summary>

This is a rather boring driver. The clock is set by default to 10 kilohertz or 10000 hertz. You initialize this driver by calling `init_pit()`.

Before the PIT is initialized, clock behavior is undefined. Usually it's initialized to ~18.1 hertz, but it is not a reliable standard.

Recently, the uptime counter was fixed. The PIT initializes at an equal 10 KHz as mentioned above. If the uptime you see doesn't increase, it's likely because you forgot to call `void retrieve_uptime(void)`.

The PIT increments a single value, `uptime_ticks`. The uptime is calculated by multiplying ticks by 0.00001 (seconds between a tick).

The reason it's better to multiply by the float than divide by the Hz is because that triggers a compiler-defined helper (`__udivdi2`), and that comes from libgcc. The problem is: how to make that libgcc layer easily usable on any device, ARM or not? I won't.

While libgcc is an external library, it's not inherently bad to link against it. However, that does introduce cross-compilation problems. So, I decided it's better to multiply instead and get the same result.
</details>

## Graphics
<details>
<summary>Click for details</summary>

This driver is one of the longest ones. There is multiple ways to print, like putc, puts, printf, print, printk etc.

A typical 80x25 screen is made of 2000 cells or 4000 bytes. A cell has a high byte (the color) and low byte (the character). The VGA colors are the standard 16 color RGB palette.

The color byte's left hex digit is the background color and the right digit is the foreground. Usually you want 0x07 because that's the typical color used in bootloaders.

Printing a single character requires putc to check for special characters like the newline, tab, backspace etc. That's because putting them directly in the VGA text buffer at **0xB8000** will print a control character.

The codepage used is cp737. You can search it online and find what exactly gets printed.

You need column and row variables which can't go higher than 80 and 25 respectively.
These are then set to the hardware cursor position by sending 4 bytes to ports 0x3D4 and 0x3D5.

As of CkOS 0.02, a heart is printed by leveraging Codepage 737. That's achieved with 0x03, which in modern terminal emulators just prints nothing or a gibberish character. Those use UTF-8 with regular ANSI instead.

The driver goal (graphics) is misleading. There is no graphics. At least not yet. The driver is purely for text mode, without it nothing would display.
</details>

## Power
<details>
<summary>Click for details</summary>

This is the least driver-like part of this section. It contains self-explanatory functions: `poweroff()` (QEMU only), `reboot()` and `halt()`.
</details>

## Serial
<details>
<summary>Click for details</summary>
  
This is like VGA but much simpler - you just get the baud right. In this kernel it's 115200 baud. You use `sputc()` and `sgetc()` for printing and receiving characters on serial.

Serial is initialized with `serial_init()`. The divisor determines the final baud, which is 0x01 for the specified above 115200 baud.

The main reason why CkOS 0.02 doesn't support serial input is because input methods are being shifted into interrupt-based input instead of poll-based methods.

Fun fact, it is not actually bad design to `printk()` to VGA even in serial-only mode, at least not until device detection phases are implemented. This is because the VGA text buffer is just physical memory. Nothing bad happens if you just write data there.
</details>

## Port
<details>
<summary>Click for details</summary>
  
Again, not much to talk about - you use `outb()` to output a byte and `inb()` to read a byte from a specified port.

Those functions translate into inline assembly with the actual instructions, `outb` and `inb`. There are also their `outw` and `inw` for 16-bit words as well as  `outl` and `inl` for longs.
In NASM, those are just `out` and `in`.
</details>

## CPU
<details>
<summary>Click for details</summary>
This pseudo-driver is just getters, where:
  
- `struct cpufreq_s get_cpu_clk(void)`: gets CPU clock speeds for CPUs that support CPUID 16h, like base, max and bus

- `unsigned int get_cpu_clk_d(void)`: attempts to get the CPU clock via TSC

- `char *get_cpu_brand(char buffer[])`: gets the full brand string, example: `Intel(R) Core(TM) i5-6600K @ 3.500 GHz`

- `void get_cpu_vendor(void)`: saves the CPU vendor in a 13-byte string (12 bytes + NUL)

- `char *get_cpu_vendor_user(void)`: returns the friendly CPU vendor
</details>

## PC speaker
<details>
<summary>Click for details</summary>

*For the rest of this page, "PC speaker" refers to the built in buzzer in IBM PC-compatibles*
  
This driver is for driving the PC speaker's coil. We have a lot of simplification in this case; the PIT channel 2 is for specifically driving the speaker.

Enabling the speaker is as simple as setting the frequency:

```c
outb(0x43, 0xB6);
uint16_t div = 1193182/frequency;
outb(0x42, div & 0xFF);
outb(0x42, div >> 8);
```

Followed by enabling the speaker:

```c
uint8_t tmp = inb(0x61);
if ((tmp & 3) != 3) outb(0x61, tmp | 3);
```

And if you want to stop the speaker:

```c
uint8_t tmp = inb(0x61);
outb(0x61, tmp & ~3);
```

Otherwise, you'd have to manually toggle the coil energized pin. That's what used to be done in DOS-era games; this stressed the CPU a lot, and it could barely do anything else.

</details>
