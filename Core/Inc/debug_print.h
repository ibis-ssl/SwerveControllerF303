#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include <stdarg.h>
#include <stdint.h>

/* Initialize internal state. Call after HAL and MX_USART1_UART_Init. */
void debug_print_init(void);

/* printf-like non-blocking logger over huart1 DMA with double buffer. */
int p(const char * fmt, ...);

/* Optional: print raw bytes (appends no terminator). */
int debug_print_bytes(const uint8_t * data, uint16_t len);

#endif /* DEBUG_PRINT_H */
