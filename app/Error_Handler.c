#include "error_handler.h"
#include "uart_logger.h"
#include "system_init.h"
#include "diagnostics.h"
#include "gpio_driver.h"
#include "S32K344.h"
#include "FreeRTOS.h"
#include "task.h"

static ErrorCode last_error = ERROR_NONE;

void ErrorHandler_Init(void) {
    last_error = ERROR_NONE;
}

void ErrorHandler_Log(ErrorCode code) {
    last_error = code;

    switch (code) {
        case ERROR_SPI_COMM:         UART_Log("[ERROR] SPI Communication Failure\r\n"); break;
        case ERROR_PEC_MISMATCH:     UART_Log("[ERROR] PEC15 Mismatch\r\n"); break;
        case ERROR_OVERVOLTAGE:      UART_Log("[ERROR] Overvoltage\r\n"); break;
        case ERROR_UNDERVOLTAGE:     UART_Log("[ERROR] Undervoltage\r\n"); break;
        case ERROR_OVERTEMP:         UART_Log("[ERROR] Overtemperature\r\n"); break;
        case ERROR_WATCHDOG_FAIL:    UART_Log("[ERROR] Watchdog Refresh Failed\r\n"); break;
        case ERROR_DISCHARGE_TIMEOUT:UART_Log("[ERROR] Discharge Timeout\r\n"); break;
        case ERROR_COMM_FAILURE:     UART_Log("[ERROR] Communication to Host Failed\r\n"); break;
        case ERROR_UNKNOWN:          UART_Log("[ERROR] Unknown Fault\r\n"); break;
        default: break;
    }
}

void ErrorHandler_TryRecover(void) {
    UART_Log("[RECOVERY] Attempting recovery...\r\n");

    if (!System_Initialize()) {
        UART_Log("[RECOVERY] Re-initialization failed\r\n");
        ErrorHandler_SafeShutdown();
        return;
    }

    if (!Run_AllDiagnostics()) {
        UART_Log("[RECOVERY] Diagnostics failed\r\n");
        ErrorHandler_SafeShutdown();
        return;
    }

    UART_Log("[RECOVERY] Recovery successful — resuming monitoring\r\n");
}

void ErrorHandler_SafeShutdown(void) {
    UART_Log("[SHUTDOWN] Entering safe shutdown mode\r\n");

    // Step 1: Open contactors (disconnect battery)
    UART_Log("[SHUTDOWN] Opening main contactors...\r\n");
    Contactor_OpenMain();  // Function in gpio_driver to control contactor pin
    Contactor_OpenPrecharge();  // Optionally open precharge relay
    Contactor_OpenCharge();  // Optionally open charging relay
    UART_Log("[SHUTDOWN] Contactors open\r\n");

    // Step 2: Reset the MCU
    UART_Log("[SHUTDOWN] System will now reset\r\n");
    NVIC_SystemReset();
}
