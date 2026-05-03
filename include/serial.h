#pragma once
#ifndef SERIAL_H
#define SERIAL_H
extern unsigned short UART1;
extern unsigned short UART2;
void serial_init();
void serial_shutdown();
void sputs(const char *s);
void sputc(char c);
int sgetc_raw(unsigned char port);
char sgetc(unsigned char port);
#endif
