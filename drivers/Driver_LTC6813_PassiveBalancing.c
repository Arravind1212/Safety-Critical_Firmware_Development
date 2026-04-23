#include "ltc6813_driver.h"
#include "spi_driver.h"
#include "pec15.h"
#include "uart_logger.h"
#include <string.h>

#define NUM_CELL_GROUPS 6  // Group A–F = 18 cells / 3 per group
#define CELL_SCALE 0.1f    // 0.1 mV per LSB

static void send_command(uint16_t cmd) {
    uint8_t tx[4];
    tx[0] = (cmd >> 8) & 0xFF;
    tx[1] = cmd & 0xFF;
    uint16_t pec = PEC15_Calculate(tx, 2);
    tx[2] = (pec >> 8) & 0xFF;
    tx[3] = pec & 0xFF;

    SPI_SetCSLow();
    SPI_TransmitReceive(tx, NULL, 4);
    SPI_SetCSHigh();
}

void LTC6813_Wake(void) {
    uint8_t dummy[2] = {0x00, 0x00};
    SPI_SetCSLow();
    SPI_TransmitReceive(dummy, NULL, 2);
    SPI_SetCSHigh();
}

void LTC6813_ClearAllRegisters(void) {
    send_command(0x0711); // CLRCELL
    send_command(0x0722); // CLRAUX
    send_command(0x0733); // CLRSTAT
}

bool LTC6813_WriteDefaultConfig(void) {
    uint8_t config[6] = { 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint16_t pec = PEC15_Calculate(config, 6);

    uint8_t tx[8];
    memcpy(tx, config, 6);
    tx[6] = (pec >> 8) & 0xFF;
    tx[7] = pec & 0xFF;

    send_command(0x0001); // WRCFG

    SPI_SetCSLow();
    SPI_TransmitReceive(tx, NULL, 8);
    SPI_SetCSHigh();

    return true;
}

bool LTC6813_ReadConfig(uint8_t *rx_config) {
    send_command(0x0002); // RDCFG

    uint8_t rx[8];
    SPI_SetCSLow();
    SPI_TransmitReceive(NULL, rx, 8);
    SPI_SetCSHigh();

    uint16_t received_pec = (rx[6] << 8) | rx[7];
    uint16_t calculated_pec = PEC15_Calculate(rx, 6);

    if (received_pec != calculated_pec) {
        UART_Log("[LTC6813] PEC mismatch on config read\r\n");
        return false;
    }

    memcpy(rx_config, rx, 6);
    return true;
}

void LTC6813_StartCellADC(void) {
    send_command(0x0260); // ADCV: Normal mode, all cells
}

bool LTC6813_ReadCellGroup(uint8_t group, uint16_t *voltages_mv) {
    if (group >= NUM_CELL_GROUPS) return false;

    uint16_t cmd = 0x0400 + (group << 4); // RDCVA, RDCVB, ..., RDCVF
    send_command(cmd);

    uint8_t rx[8];
    SPI_SetCSLow();
    SPI_TransmitReceive(NULL, rx, 8);
    SPI_SetCSHigh();

    uint16_t received_pec = (rx[6] << 8) | rx[7];
    uint16_t calculated_pec = PEC15_Calculate(rx, 6);
    if (received_pec != calculated_pec) {
        UART_Log("[LTC6813] PEC mismatch on cell group read\r\n");
        return false;
    }

    for (int i = 0; i < 3; i++) {
        uint16_t raw = rx[2 * i] | (rx[2 * i + 1] << 8);
        voltages_mv[i] = (uint16_t)(raw * CELL_SCALE);
    }

    return true;
}

bool LTC6813_ReadAllCellVoltages(uint16_t *all_cells_mv) {
    for (uint8_t group = 0; group < NUM_CELL_GROUPS; group++) {
        if (!LTC6813_ReadCellGroup(group, &all_cells_mv[group * 3])) {
            UART_Log("[LTC6813] Failed to read cell group ");
            UART_LogUint("Group", group);
            return false;
        }
    }
    return true;
}
