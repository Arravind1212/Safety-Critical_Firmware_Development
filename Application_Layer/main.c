#include "system_init.h"
#include "diagnostics.h"
#include "error_handler.h"
#include "gpio_driver.h"
#include "spi_driver.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

// Task headers
#include "cell_monitor.h"
#include "watchdog_task.h"
#include "discharge_timer.h"
#include "host_comm_task.h"

int main(void) {
    // Hardware Initialization
    UART_Init();
    SPI_Init();
    GPIO_InitTempSensors();
    ErrorHandler_Init();

    UART_Log("\r\n=== BMS Firmware Boot ===\r\n");

    // Step 1: System Initialization
    if (!System_Initialize()) {
        ErrorHandler_Log(ERROR_SPI_COMM);
        ErrorHandler_SafeShutdown();
    }

    // Step 2: Run Startup Diagnostics
    if (!Run_AllDiagnostics()) {
        ErrorHandler_Log(ERROR_UNKNOWN);
        ErrorHandler_SafeShutdown();
    }

    // Step 3: Start RTOS Tasks
    UART_Log("[RTOS] Creating tasks...\r\n");

    xTaskCreate(CellMonitor_Task,       "CellMonitor",       512, NULL, 2, NULL);
    xTaskCreate(WatchdogRefresh_Task,   "WatchdogRefresh",   256, NULL, 2, NULL);
    xTaskCreate(DischargeTimer_Task,    "DischargeTimer",    256, NULL, 1, NULL);
    xTaskCreate(HostComm_Task,          "HostComm",          512, NULL, 1, NULL);

    UART_Log("[RTOS] Starting scheduler...\r\n");
    vTaskStartScheduler();

    // End
    while (1) { }
}






