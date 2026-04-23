#include "spi_driver.h"
#include "S32K344.h"
#include "device_registers.h"
#include "FreeRTOS.h"
#include "task.h"

#define SPI_CS_GPIO      PTD
#define SPI_CS_PIN       3

static TaskHandle_t spiTaskToNotify = NULL;
static volatile uint8_t *txBuffer = NULL;
static volatile uint8_t *rxBuffer = NULL;
static volatile uint16_t spiLength = 0;
static volatile uint16_t spiIndex = 0;

void SPI_Init(void) {
    // Enable clocks
    PCC->PCCn[PCC_DSPI0_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    // SPI pin muxing: PTD0-SCK, PTD1-MOSI, PTD2-MISO, PTD3-CS
    PORTD->PCR[0] = PORT_PCR_MUX(2);  // SCK
    PORTD->PCR[1] = PORT_PCR_MUX(2);  // MOSI
    PORTD->PCR[2] = PORT_PCR_MUX(2);  // MISO
    PORTD->PCR[3] = PORT_PCR_MUX(1);  // CS - GPIO

    PTD->PDDR |= (1 << SPI_CS_PIN);  // CS as output
    SPI_SetCSHigh();

    // LPSPI0 init
    LPSPI0->CR = 0;
    LPSPI0->IER = 0;
    LPSPI0->DER = 0;
    LPSPI0->CFGR1 = LPSPI_CFGR1_MASTER(1);  // Master mode
    LPSPI0->CCR = LPSPI_CCR_SCKDIV(8);      // Baud rate divider
    LPSPI0->TCR = LPSPI_TCR_FRAMESZ(7);     // 8-bit frame

    LPSPI0->CR = LPSPI_CR_MEN_MASK;         // Enable SPI
    NVIC_EnableIRQ(LPSPI0_IRQn);            // Enable IRQ
}

bool SPI_TransmitReceive(uint8_t *txData, uint8_t *rxData, uint16_t length) {
    if (length == 0 || txData == NULL || rxData == NULL) return false;

    txBuffer = txData;
    rxBuffer = rxData;
    spiLength = length;
    spiIndex = 0;

    spiTaskToNotify = xTaskGetCurrentTaskHandle();

    SPI_SetCSLow();  // Begin SPI

    // Trigger first transfer manually
    LPSPI0->TDR = txBuffer[spiIndex];
    LPSPI0->SR |= LPSPI_SR_TDF_MASK;
    LPSPI0->IER = LPSPI_IER_RDF_RE_MASK;  // Enable receive interrupt

    // Wait until ISR notifies task (with timeout)
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10)) == 0) {
        SPI_SetCSHigh();
        return false;  // Timeout
    }

    SPI_SetCSHigh();
    return true;
}

void LPSPI0_IRQHandler(void) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if ((LPSPI0->SR & LPSPI_SR_RDF_MASK) && (spiIndex < spiLength)) {
        rxBuffer[spiIndex] = (uint8_t)(LPSPI0->RDR & 0xFF);
        spiIndex++;

        if (spiIndex < spiLength) {
            LPSPI0->TDR = txBuffer[spiIndex];
            LPSPI0->SR |= LPSPI_SR_TDF_MASK;
        } else {
            // Transfer complete
            LPSPI0->IER &= ~LPSPI_IER_RDF_RE_MASK;  // Disable interrupt
            vTaskNotifyGiveFromISR(spiTaskToNotify, &higherPriorityTaskWoken);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }

        LPSPI0->SR |= LPSPI_SR_RDF_MASK;  // Clear RDF flag
    }
}

void SPI_SetCSLow(void) {
    PTD->PCOR |= (1 << SPI_CS_PIN);
}

void SPI_SetCSHigh(void) {
    PTD->PSOR |= (1 << SPI_CS_PIN);
}
