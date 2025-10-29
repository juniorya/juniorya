#ifndef DRIVERS_UART_H
#define DRIVERS_UART_H

#include <stdint.h>
#include <stdbool.h>

void uart_init(uint32_t baudrate);
void uart_write(const char *str);
int  uart_read_line(char *buffer, int max_len);

#endif
