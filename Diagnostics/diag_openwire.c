#include "diag_openwire.h"
#include "ltc6813_driver.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_OpenWire(void) {
    UART_Log("[OPENWIRE] Running open-wire test (ADOW)...\r\n");

    LTC6813_StartOpenWireConversion(); // ADOW command
    vTaskDelay(pdMS_TO_TICKS(3));

    uint16_t cell_v[3];
    if (!LTC6813_ReadCellGroup(0, cell_v)) return false;

    for (int i = 0; i < 3; i++) {
        if (cell_v[i] < 100) {
            UART_Log("[OPENWIRE] Detected open wire on cell input\r\n");
            return false;
        }
    }

    UART_Log("[OPENWIRE] All cell inputs connected\r\n");
    return true;
}

