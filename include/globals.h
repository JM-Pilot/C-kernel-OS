#ifndef GLOBALS
#define GLOBALS
#include <stdlib.h>
#include <stdint.h>
extern double uptime;
extern const char *ver;
extern const char *logo;
extern const char *credits;
extern char serial_in;
extern char serial_out;
extern volatile char kbc;
extern volatile uint64_t uptime_ticks;
extern unsigned char loglevel;

// Print levels
#define PR_EMERG 0
#define PR_CRIT 1
#define PR_ERROR 2
#define PR_WARN 3
#define PR_INFO 4
#define PR_DETAIL 5
#define PR_DEBUG 6
#define PR_UNNECESSARY 7
#endif
