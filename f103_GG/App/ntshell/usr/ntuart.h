#ifndef _NTUART_H
#define _NTUART_H

#include "usart.h"

uint8_t uart_getc(void);
void uart_putc(uint8_t c);
void uart_puts(char *str);
int serial_read(char *buf, int cnt, void *extobj);
int serial_write(const char *buf, int cnt, void *extobj);

#endif
