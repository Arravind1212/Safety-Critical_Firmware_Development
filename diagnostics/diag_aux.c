#include "diag_aux.h"
#include "ltc6813_driver.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_AUX(void) {
    UART_Log("[AUX] Starting AUX conversion...\r\n");

    LTC6813_StartAUXADC(); // ADAX command (GPIOs/Vref2/Vref1)
    vTaskDelay(pdMS_TO_TICKS(3)); // Wait ~2.5ms conversion

    uint16_t aux_data[3];
    if (!LTC6813_ReadAuxGroup(0, aux_data)) {
        UART_Log("[AUX] Failed to read AUX group\r\n");
        return false;
    }

    UART_Log("[AUX] AUX data read successfully\r\n");
    return true;
}

