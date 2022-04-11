#include "ntuart.h"

uint8_t uart_getc(void)
{
    uint8_t c = 0;
    while(1){
        if (HAL_UART_Receive_IT(&huart3, &c, 1) == HAL_OK) {
            return c;
        } 
    }
}

void uart_putc(uint8_t c)
{
    while (1) {
        if (HAL_UART_Transmit_IT(&huart3, &c, 1) == HAL_OK) {
            return;
        }
    }
}

void uart_puts(char *str)
{
    while (*str) {
        uart_putc(*str++);
    }
}

int serial_read(char *buf, int cnt, void *extobj)
{
    for (int i = 0; i < cnt; i++) {
        buf[i] = uart_getc();
    }
    return cnt;
}

int serial_write(const char *buf, int cnt, void *extobj)
{
    for (int i = 0; i < cnt; i++) {
        uart_putc(buf[i]);
    }
    return cnt;
}


