#include "uart_driver.h"
#include "S32K344.h"
#include "device_registers.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "string.h"

#define UART_MODULE          LPUART1
#define UART_IRQn            LPUART1_RxTx_IRQn
#define UART_IRQ_PRIORITY    3
#define TX_BUFFER_SIZE       128

static QueueHandle_t uart_rx_queue;
static volatile const char *tx_str_ptr = NULL;
static volatile uint32_t tx_index = 0;

// Initialize UART1 on default pins (e.g., PTD6 - TX, PTD7 - RX)
void UART_Init(void) {
    // Enable clock to LPUART1 and PORTD
    PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    // Configure pins for UART
    PORTD->PCR[6] = PORT_PCR_MUX(2); // TX
    PORTD->PCR[7] = PORT_PCR_MUX(2); // RX

    // Disable UART before config
    UART_MODULE->CR1 &= ~LPUART_CR1_TE_MASK;
    UART_MODULE->CR1 &= ~LPUART_CR1_RE_MASK;

    UART_MODULE->BAUD = 0x0F00;  // Baud rate: 115200 (example for 80 MHz source)
    UART_MODULE->CTRL = 0;
    UART_MODULE->CR1 = LPUART_CR1_TE_MASK | LPUART_CR1_RE_MASK |
                       LPUART_CR1_RIE_MASK | LPUART_CR1_TIE_MASK;

    NVIC_SetPriority(UART_IRQn, UART_IRQ_PRIORITY);
    NVIC_EnableIRQ(UART_IRQn);

    uart_rx_queue = xQueueCreate(16, sizeof(uint8_t));
}

// Blocking send
void UART_SendChar(char c) {
    while (!(UART_MODULE->STAT & LPUART_STAT_TDRE_MASK));
    UART_MODULE->DATA = c;
}

// Blocking send string
void UART_SendString(const char *str) {
    while (*str) {
        UART_SendChar(*str++);
    }
}

// Asynchronous send (non-blocking start)
bool UART_SendAsync(const char *str) {
    if (tx_str_ptr != NULL) return false;  // Already transmitting

    tx_str_ptr = str;
    tx_index = 0;

    // Enable transmit interrupt
    UART_MODULE->CTRL |= LPUART_CTRL_TIE_MASK;
    return true;
}

// Receive (non-blocking)
bool UART_ReadChar(uint8_t *c) {
    return xQueueReceive(uart_rx_queue, c, 0) == pdPASS;
}

// UART interrupt handler
void LPUART1_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // RX Interrupt
    if ((UART_MODULE->STAT & LPUART_STAT_RDRF_MASK)) {
        uint8_t rx = UART_MODULE->DATA;
        xQueueSendFromISR(uart_rx_queue, &rx, &xHigherPriorityTaskWoken);
    }

    // TX Interrupt (asynchronous sending)
    if ((UART_MODULE->STAT & LPUART_STAT_TDRE_MASK) && tx_str_ptr) {
        if (tx_str_ptr[tx_index] != '\0') {
            UART_MODULE->DATA = tx_str_ptr[tx_index++];
        } else {
            // End of string
            tx_str_ptr = NULL;
            tx_index = 0;
            UART_MODULE->CTRL &= ~LPUART_CTRL_TIE_MASK;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}