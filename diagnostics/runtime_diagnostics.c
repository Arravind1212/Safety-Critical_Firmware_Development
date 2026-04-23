#include "runtime_diagnostics.h"

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "diag_spi.h"
#include "diag_gpio.h"
#include "diag_watchdog.h"
#include "diag_openwire.h"
#include "diag_short.h"
#include "diag_redundancy.h"
#include "uart_logger.h"
#include "error_handler.h"

bool RuntimeDiagnostics_RunPeriodic(void)
{
    bool status = true;

    /* Communication health */
    if (!Diag_SpiCheck())
    {
        UART_Log("[RUNTIME DIAG] SPI check failed\r\n");
        ErrorHandler_Report(ERROR_SPI_DIAG_FAIL);
        status = false;
    }

    /* Sensor plausibility */
    if (!Diag_GpioCheck())
    {
        UART_Log("[RUNTIME DIAG] GPIO/sensor check failed\r\n");
        ErrorHandler_Report(ERROR_GPIO_DIAG_FAIL);
        status = false;
    }

    /* Watchdog health */
    if (!Diag_WatchdogCheck())
    {
        UART_Log("[RUNTIME DIAG] Watchdog check failed\r\n");
        ErrorHandler_Report(ERROR_WATCHDOG_DIAG_FAIL);
        status = false;
    }

    /* Connection integrity */
    if (!Diag_OpenWireCheck())
    {
        UART_Log("[RUNTIME DIAG] Open-wire check failed\r\n");
        ErrorHandler_Report(ERROR_OPENWIRE_DIAG_FAIL);
        status = false;
    }

    /* Abnormal low-value / short check */
    if (!Diag_ShortCheck())
    {
        UART_Log("[RUNTIME DIAG] Short-circuit check failed\r\n");
        ErrorHandler_Report(ERROR_SHORT_DIAG_FAIL);
        status = false;
    }

    /* Measurement stability */
    if (!Diag_RedundancyCheck())
    {
        UART_Log("[RUNTIME DIAG] Redundancy check failed\r\n");
        ErrorHandler_Report(ERROR_REDUNDANCY_DIAG_FAIL);
        status = false;
    }

    return status;
}

void RuntimeDiagnostics_Task(void *argument)
{
    (void)argument;

    UART_Log("[RUNTIME DIAG] Task started\r\n");

    for (;;)
    {
        bool ok = RuntimeDiagnostics_RunPeriodic();

        if (!ok)
        {
            UART_Log("[RUNTIME DIAG] Fault detected during runtime diagnostics\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}