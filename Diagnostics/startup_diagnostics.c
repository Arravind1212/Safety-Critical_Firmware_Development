#include "startup_diagnostics.h"
#include "diag_spi.h"
#include "diag_crc.h"
#include "diag_gpio.h"
#include "diag_aux.h"
#include "diag_watchdog.h"
#include "diag_selftest.h"
#include "diag_redundancy.h"
#include "diag_openwire.h"
#include "diag_short.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"

// Run all diagnostics sequentially
bool Run_AllDiagnostics(void) {
    UART_Log("\r\n[DIAGNOSTICS] Running startup safety checks...\r\n");

    if (!Diag_SpiCheck())          { UART_Log("[FAIL] SPI Check\r\n"); return false; }
    if (!Diag_CrcCheck())          { UART_Log("[FAIL] PEC Check\r\n"); return false; }
    if (!Diag_GpioCheck())         { UART_Log("[FAIL] GPIO Check\r\n"); return false; }
    if (!Diag_AuxCheck())          { UART_Log("[FAIL] AUX ADC Check\r\n"); return false; }
    if (!Diag_WatchdogCheck())     { UART_Log("[FAIL] Watchdog Check\r\n"); return false; }
    if (!Diag_SelfTestCheck())     { UART_Log("[FAIL] Self-Test Check\r\n"); return false; }
    if (!Diag_RedundancyCheck())   { UART_Log("[FAIL] Redundancy Check\r\n"); return false; }
    if (!Diag_OpenWireCheck())     { UART_Log("[FAIL] Open Wire Check\r\n"); return false; }
    if (!Diag_ShortCheck()) { UART_Log("[FAIL] Short Circuit Check\r\n"); return false; }

    UART_Log("[DIAGNOSTICS] All checks passed.\r\n");
    return true;
}
