#include "diag_short.h"
#include "ltc6813_driver.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_ShortCircuit(void) {
    UART_Log("[SHORT] Checking for shorted cells...\r\n");

    LTC6813_StartCellADC();  // ADCV
    vTaskDelay(pdMS_TO_TICKS(3));

    uint16_t v[3];
    if (!LTC6813_ReadCellGroup(0, v)) return false;

    for (int i = 0; i < 3; i++) {
        if (v[i] < 50) {
            UART_Log("[SHORT] Short detected: cell voltage < 50mV\r\n");
            return false;
        }
    }

    UART_Log("[SHORT] No shorts detected\r\n");
    return true;
}


