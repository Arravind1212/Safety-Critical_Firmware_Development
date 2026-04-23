#include "diag_watchdog.h"
#include "ltc6813_driver.h"
#include "FreeRTOS.h"
#include "task.h"

static bool Check_Watchdog(void) {
    UART_Log("[WDT] Performing watchdog refresh test...\r\n");

    // Step 1: Wake LTC6813 to reset WDT timer
    LTC6813_Wake();

    // Step 2: Wait and attempt a valid command
    vTaskDelay(pdMS_TO_TICKS(5)); // Short wait

    // Step 3: Try to write config
    bool result = LTC6813_WriteDefaultConfig();

    if (!result) {
        UART_Log("[WDT] Watchdog test failed — LTC6813 did not respond\r\n");
        return false;
    }

    UART_Log("[WDT] Watchdog is alive and responsive\r\n");
    return true;
}