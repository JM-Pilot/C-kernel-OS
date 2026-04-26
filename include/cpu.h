#ifndef CPU_H
#define CPU_H
char *get_cpu_vendor(); // cpu.asm
char *get_cpu_vendor_user(); // cpu_user.c
char *get_cpu_brand(char buffer[]); // cpu_user.c
struct cpufreq_s {
	unsigned int base;
	unsigned int max;
	unsigned int bus;
};
struct cpufreq_s get_cpu_clk();
#endif
