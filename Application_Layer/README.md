## Application_Layer
This is the firmware application layer.

It contains the following main system behavior.
## main.c
- Program entry
- Initialize MCU/peripherals
- Create RTOS tasks
- Start scheduler

## system_init.c
- Startup sequence
- Wake external device
- Clear registers
- Load default configuration
- Call startup diagnostics

## sensor_monitor.c
- Periodic data read
- Convert raw ADC values
- Threshold checks
- Trigger control actions

## control_logic.c
- Control decisions based on measurements
※ For example threshold-based control, actuator enable/disable, balancing-like logic

## watchdog_task.c
- Periodic watchdog refresh
- Timeout handling

## comm_task.c
- Send logs / status through UART or CAN
- Package data for transmission

## error_handler.c
- Central error processing
- Recovery attempt
- Safe shutdown if needed

--END--
