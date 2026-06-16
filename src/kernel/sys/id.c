#include <kernel/sys/globals.h>

#ifdef __STDC_VERSION__
const int c_version = __STDC_VERSION__;
#else
const int c_version = 0;
#endif
#if defined(__GNUC__)
const char *compiler = "clang";
const int compiler_ver[3] = {__clang_major__, __clang_minor__, __clang_patchlevel__};
#elif defined(__GNUC__)
const char *compiler = "gcc";
const int compiler_ver[3] = {__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__};
#define XSTR(x) STR(x)
#define STR(x) #x
#else
const char *compiler = "unknown";
const int compiler_ver[3] = {0, 0, 0};
#endif
