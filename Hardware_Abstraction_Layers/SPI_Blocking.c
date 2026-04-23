#include "spi_driver.h"
#include "S32K344.h"
#include "device_registers.h"

#define SPI_CS_PORT PTD
#define SPI_CS_PIN  3

void SPI_Init(void) {
    // Enable clock for PORTD and LPSPI0
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_DSPI0_INDEX] |= PCC_PCCn_CGC_MASK;

    // Configure SPI pins (ALT2 function for LPSPI0)
    PORTD->PCR[0] = PORT_PCR_MUX(2); // PTD0 - SCK
    PORTD->PCR[1] = PORT_PCR_MUX(2); // PTD1 - MOSI
    PORTD->PCR[2] = PORT_PCR_MUX(2); // PTD2 - MISO
    PORTD->PCR[3] = PORT_PCR_MUX(1); // PTD3 - CS (GPIO)

    // Set CS pin as output
    SPI_CS_PORT->PDDR |= (1 << SPI_CS_PIN);
    SPI_SetCSHigh(); // Default high (inactive)

    // Reset and configure LPSPI0
    LPSPI0->CR = 0; // Disable module to configure
    LPSPI0->IER = 0;
    LPSPI0->DER = 0;
    LPSPI0->CFGR1 = LPSPI_CFGR1_MASTER(1); // Master mode

    LPSPI0->CCR = LPSPI_CCR_SCKDIV(8); // SCK = SourceClock / (2 * (SCKDIV + 2))
    LPSPI0->TCR = LPSPI_TCR_FRAMESZ(7); // 8-bit frame

    LPSPI0->CR = LPSPI_CR_MEN_MASK; // Enable LPSPI0
}

bool SPI_TransmitReceive(uint8_t *txData, uint8_t *rxData, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        while (!(LPSPI0->SR & LPSPI_SR_TDF_MASK)); // Wait TX ready
        LPSPI0->TDR = txData ? txData[i] : 0xFF;
        LPSPI0->SR |= LPSPI_SR_TDF_MASK;

        while (!(LPSPI0->SR & LPSPI_SR_RDF_MASK)); // Wait RX ready
        if (rxData) rxData[i] = (uint8_t)(LPSPI0->RDR & 0xFF);
        LPSPI0->SR |= LPSPI_SR_RDF_MASK;
    }
    return true;
}

void SPI_SetCSLow(void) {
    SPI_CS_PORT->PCOR |= (1 << SPI_CS_PIN);
}

void SPI_SetCSHigh(void) {
    SPI_CS_PORT->PSOR |= (1 << SPI_CS_PIN);
}
