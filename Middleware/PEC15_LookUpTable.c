#include "diag_crc.h"
#include "pec15.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

// Method 1: PEC15 Check → Calculate CRC on known data and validate
uint16_t PEC15_Calculate_Table(const uint8_t *data, uint8_t len) {
    uint16_t remainder = 16;  // Starting value 0x0010
    for (uint8_t i = 0; i < len; i++) {
        uint8_t index = ((remainder >> 7) ^ data[i]) & 0xFF;
        remainder = (remainder << 8) ^ pec15_table[index];
    }
    return (remainder >> 1);  // Output is 15-bit PEC
}