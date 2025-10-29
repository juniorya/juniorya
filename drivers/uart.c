#include "uart.h"
#include <string.h>

#define UART_BUFFER_SIZE 256

static struct {
    char rx_buffer[UART_BUFFER_SIZE];
    int rx_length;
} s_uart;

void uart_init(uint32_t baudrate)
{
    (void)baudrate;
    s_uart.rx_length = 0;
}

void uart_write(const char *str)
{
    (void)str;
    /* integration point for console output */
}

int uart_read_line(char *buffer, int max_len)
{
    if (s_uart.rx_length == 0) {
        return 0;
    }
    int len = s_uart.rx_length < max_len - 1 ? s_uart.rx_length : max_len - 1;
    memcpy(buffer, s_uart.rx_buffer, (size_t)len);
    buffer[len] = '\0';
    s_uart.rx_length = 0;
    return len;
}
