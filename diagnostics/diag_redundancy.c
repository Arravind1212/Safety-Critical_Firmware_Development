#include "diag_redundancy.h"
#include "ltc6813_driver.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_Redundancy(void) {
    UART_Log("[REDUNDANCY] Performing redundant voltage comparison...\r\n");

    uint16_t v1[3], v2[3];
    LTC6813_StartCellADC();
    vTaskDelay(pdMS_TO_TICKS(3));
    if (!LTC6813_ReadCellGroup(0, v1)) return false;

    LTC6813_StartCellADC(); // Second conversion
    vTaskDelay(pdMS_TO_TICKS(3));
    if (!LTC6813_ReadCellGroup(0, v2)) return false;

    for (int i = 0; i < 3; i++) {
        uint16_t diff = (v1[i] > v2[i]) ? (v1[i] - v2[i]) : (v2[i] - v1[i]);
        if (diff > 10) { // 10 mV difference considered suspicious
            UART_Log("[REDUNDANCY] Voltage mismatch detected\r\n");
            return false;
        }
    }

    UART_Log("[REDUNDANCY] Voltage measurements consistent\r\n");
    return true;
}
