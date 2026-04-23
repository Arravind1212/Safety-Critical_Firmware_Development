#include "diag_crc.h"
#include "pec15.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

// Method 2: PEC15 Check → Calculated via ADI's PEC15 polynomial
// Polynomial for PEC15: x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1 (0x4599)
#define PEC15_POLY 0x4599

uint16_t PEC15_Calculate(const uint8_t *data, uint8_t len) {
    uint16_t remainder = 16;  // Initial value is 0x0010 (as per datasheet)
    for (uint8_t i = 0; i < len; i++) {
        remainder ^= ((uint16_t)data[i] << 7);  // Align data byte to top 8 bits

        for (uint8_t bit = 0; bit < 8; bit++) {
            if (remainder & 0x4000) {  // If bit 14 is set
                remainder = (remainder << 1) ^ PEC15_POLY;
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    return (remainder >> 1);  // PEC15 result is top 15 bits
}

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