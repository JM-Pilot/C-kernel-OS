#include <globals.h>
#include <stdlib.h>
#include <stdint.h>
double uptime = 0.0;
const char *ver = "CkOS Beta Version 0.03 for i386 (x86-32) at 2nd April 2026";
const char *logo =
" _   _  _\n"
"/ |/| ||_`\n"
"\\_|\\|_|._|";
const char *credits =
"Credits:\n"
"        axrxvm: numerous helpful resources about aOS\n"
"      oslayout: gave me a copy of PotatoOS for studying\n"
"           you: for usage of this experimental OS";
char serial_in = false; // true and false is defined in stdlib.h. while this is not the C standard, it makes you have to import less headers.
char serial_out = false;
volatile char kbc = 0;
// i'm sorry fellow C developers for this shitty solution but i just have to do this
uint64_t uptime_ticks = 0;

unsigned char loglevel = 7; // You don't want to recompile the kernel just because you don't have all the info to debug an error
