#include "system_init.h"
#include "ltc6813_driver.h"
#include "uart_logger.h"

bool System_Initialize(void) {
    UART_Log("\r\n[SYSTEM INIT] Starting system initialization...\r\n");

    // Step 1: Wake LTC6813 (via isoSPI dummy pulse)
    UART_Log("[STEP 1] Wake LTC6813\r\n");
    LTC6813_Wake();

    // Step 2: Clear internal registers
    UART_Log("[STEP 2] Clear all cell/AUX/status registers\r\n");
    LTC6813_ClearAllRegisters();

    // Step 3: Write default configuration
    UART_Log("[STEP 3] Write LTC6813 configuration register\r\n");
    if (!LTC6813_WriteDefaultConfig()) {
        UART_Log("[ERROR] LTC6813 config write failed!\r\n");
        return false;
    }

    UART_Log("[SYSTEM INIT] Initialization complete.\r\n");
    return true;
}