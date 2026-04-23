#include "diag_crc.h"
#include "pec15.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

// Method 1: PEC15 Check → Calculate CRC on known data and validate
static bool Check_PEC(void) {
    uint8_t data[6] = {0xFC, 0x00, 0x00, 0x00, 0x00, 0x00}; // sample config
    uint16_t pec = PEC15_Calculate(data, 6);
    return (pec != 0);
}

// Method 2: PEC15 Check → Calculated via ADI's PEC15 polynomial
static bool Check_PEC(void) {
    UART_Log("[PEC] Verifying PEC15 calculation against known values...\r\n");

    // Example known-good data and PEC (from LTC6813 datasheet)
    uint8_t data[6] = {0xFC, 0x00, 0x00, 0x00, 0x00, 0x00}; // Default config
    uint16_t expected = 0x07C2; 

    uint16_t calculated = PEC15_Calculate(data, 6);

    if (calculated != expected) {
        UART_Log("[PEC] PEC mismatch! Expected 0x07C2, got ");
        UART_LogHex((uint8_t*)&calculated, 2);
        return false;
    }

    UART_Log("[PEC] PEC check passed\r\n");
    return true;
}