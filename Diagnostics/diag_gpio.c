#include "diag_gpio.h"
#include "ltc6813_driver.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_GPIO(void) {
    UART_Log("[GPIO/NTC] Measuring NTC voltage via AUX ADC...\r\n");

    LTC6813_StartAUXADC(); // Start ADAX conversion
    vTaskDelay(pdMS_TO_TICKS(3));

    uint16_t aux[3];
    if (!LTC6813_ReadAuxGroup(0, aux)) return false;

    for (int i = 0; i < 2; i++) {
        UART_LogUint16("NTC_AUX[mV]", aux[i]);
        if (aux[i] < 200 || aux[i] > 3000) {
            UART_Log("[GPIO/NTC] Abnormal temp sensor voltage\r\n");
            return false;
        }
    }

    UART_Log("[GPIO/NTC] NTC voltages in range\r\n");
    return true;
}