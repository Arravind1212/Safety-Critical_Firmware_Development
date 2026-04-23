#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// UART initialization
void UART_Init(void);

// Transmit one character (blocking)
void UART_SendChar(char c);

// Transmit string (blocking)
void UART_SendString(const char *str);

// Interrupt-based transmit (optional)
bool UART_SendAsync(const char *str);

// Receive one character (non-blocking, returns true if character received)
bool UART_ReadChar(uint8_t *c);

// UART Interrupt Handler (called from vector table)
void LPUART1_IRQHandler(void);

#endif // UART_DRIVER_H