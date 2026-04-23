// Cell monitor.c (With active cell balancing)

#include "cell_monitor.h"
#include "ltc6813_driver.h"
#include "gpio_driver.h"
#include "uart_logger.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

static uint16_t cell_voltage_mV[CELL_COUNT];
static uint8_t ntc_voltage_mV[TEMP_SENSOR_COUNT];
static uint8_t sctrl_data[6] = {0}; // For S control register

// Simple balancing: discharge all cells above average by > threshold
static void ActiveCellBalancing(void) {
    memset(sctrl_data, 0, sizeof(sctrl_data));
    
    uint32_t total = 0;
    for (int i = 0; i < CELL_COUNT; i++) {
        total += cell_voltage_mV[i];
    }
    uint16_t avg_voltage = total / CELL_COUNT;

    for (int i = 0; i < CELL_COUNT; i++) {
        if (cell_voltage_mV[i] > avg_voltage + BALANCE_THRESHOLD_MV) {
            if (i < 8)
                sctrl_data[4] |= (1 << i); // S control A
            else
                sctrl_data[5] |= (1 << (i - 8)); // S control B
        }
    }

    if (LTC6813_WriteSControl(sctrl_data)) {
        UART_Log("[BALANCING] Active balancing initiated via S pin\r\n");
    } else {
        UART_Log("[ERROR] Failed to write S control register\r\n");
    }
}

static void ConvertTo_mV(uint16_t *raw_adc, float scale, uint16_t *out_mV, int count) {
    for (int i = 0; i < count; i++) {
        out_mV[i] = (uint16_t)(raw_adc[i] * scale);
    }
}

static void CheckSafetyAndBalance(void) {
    for (int i = 0; i < 3; i++) {
        UART_LogUint16("Cell Voltage (mV)", cell_voltage_mV[i]);
        if (cell_voltage_mV[i] > OV_THRESHOLD_MV) {
            UART_Log("[FAULT] Overvoltage detected\r\n");
            over_voltage_detected = true;

            ErrorHandler_Log(ERROR_OVERVOLTAGE);
            Send_OV_CANAlert(i + 1, cell_voltage_mV[i]);  // Alert for DTC storage / CAN log
            vTaskDelay(pdMS_TO_TICKS(2)); // Ensure logic happens within SSR1 ≤ 5ms

            // Terminate charging logic (via GPIO or signal to charger)
            GPIO_SetChargeRelay(false);
            return; // No further checks if protection triggered
        } else if (cell_voltage_mV[i] < UV_THRESHOLD_MV) {
            UART_Log("[FAULT] Undervoltage detected\r\n");
            ErrorHandler_Log(ERROR_UNDERVOLTAGE);
        }
    }

    // NTC via AUX ADC
    uint16_t aux_raw[3] = {0};
    LTC6813_StartAUXADC();
    vTaskDelay(pdMS_TO_TICKS(3));

    if (!LTC6813_ReadAuxGroup(0, aux_raw)) {
        UART_Log("[FAULT] AUX read failed\r\n");
        return;
    }

    ConvertTo_mV(aux_raw, 0.1f, ntc_voltage_mV, TEMP_SENSOR_COUNT);

    for (int i = 0; i < TEMP_SENSOR_COUNT; i++) {
        UART_LogUint16("NTC Voltage (mV)", ntc_voltage_mV[i]);
        if (ntc_voltage_mV[i] < NTC_OVERTEMP_THRESHOLD_MV) {
            UART_Log("[FAULT] Overtemperature detected via NTC\r\n");
            return;
        }
    }

    // Safe state → perform balancing
    ActiveCellBalancing();
}

void CellMonitor_Task(void *pvParameters) {
    UART_Log("[TASK] Cell Monitor started\r\n");

    while (1) {
        LTC6813_StartCellADC();
        vTaskDelay(pdMS_TO_TICKS(3));

        uint16_t raw_adc[3] = {0};
        if (!LTC6813_ReadCellGroup(0, raw_adc)) {
            UART_Log("[ERROR] Failed to read cell group\r\n");
            continue;
        }

        ConvertTo_mV(raw_adc, 0.1f, cell_voltage_mV, 3);
        CheckSafetyAndBalance();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
