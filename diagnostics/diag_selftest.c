#include "diag_selftest.h"
#include "ltc6813_driver.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_SelfTest(void) {
    UART_Log("[SELFTEST] Starting ADC self-test...\r\n");

    LTC6813_StartSelfTestADC();  // ADCV[ST] = 01
    vTaskDelay(pdMS_TO_TICKS(3));

    uint16_t test_voltages[3];
    if (!LTC6813_ReadCellGroup(0, test_voltages)) return false;

    for (int i = 0; i < 3; i++) {
        if (test_voltages[i] < 28000 || test_voltages[i] > 32000) {  // 3.0V ±10%
            UART_Log("[SELFTEST] ADC output out of expected range\r\n");
            return false;
        }
    }

    UART_Log("[SELFTEST] ADC self-test passed\r\n");
    return true;
}
