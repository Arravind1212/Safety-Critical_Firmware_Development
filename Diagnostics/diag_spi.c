#include "diag_spi.h"
#include "spi_driver.h"
#include "pec15.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

// SPI Check: Send test pattern and verify loopback
static bool Check_SPI(void) {
    UART_Log("[SPI] Sending RDCFGA to test SPI interface...\r\n");

    uint8_t rx_buf[8] = {0};

    // Send RDCFGA command: 0x0207 with PEC
    uint8_t cmd[4];
    cmd[0] = 0x02;
    cmd[1] = 0x07;
    uint16_t pec = PEC15_Calculate(cmd, 2);
    cmd[2] = (pec >> 8) & 0xFF;
    cmd[3] = pec & 0xFF;

    // Send command to LTC6813
    SPI_SetCSLow();
    SPI_TransmitReceive(cmd, NULL, 4);
    SPI_SetCSHigh();

    // Receive 6-byte config + 2-byte PEC
    SPI_SetCSLow();
    bool ok = SPI_TransmitReceive(NULL, rx_buf, 8);
    SPI_SetCSHigh();

    if (!ok) {
        UART_Log("[SPI] SPI transfer failed\r\n");
        return false;
    }

    // Verify received PEC
    uint16_t rx_pec = (rx_buf[6] << 8) | rx_buf[7];
    uint16_t calc_pec = PEC15_Calculate(rx_buf, 6);

    if (rx_pec != calc_pec) {
        UART_Log("[SPI] RDCFGA PEC check failed\r\n");
        return false;
    }

    UART_Log("[SPI] SPI communication verified via RDCFGA\r\n");
    return true;
}