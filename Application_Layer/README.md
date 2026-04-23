## Application_Layer
This is the firmware application layer.

It contains the following main system behavior.
## main.c
- Program entry
- Initialize MCU/peripherals
- Create RTOS tasks
- Start scheduler

![main.c](..docs/images/main.png)

## System_Init.c
- Startup sequence
- Wake external device
- Clear registers
- Load default configuration
- Call startup diagnostics

![System_Init.c](docs/images/System_Init.png)

## Sensor_Monitoring.c
- Periodic data read
- Convert raw ADC values
- Threshold checks
- Trigger control actions

![Sensor_Monitoring.c](docs/images/Sensor_Monitoring.png)

## Control_Logic.c
- Control decisions based on measurements
※ For example threshold-based control, actuator enable/disable, balancing-like logic

![Control_Logic.c](docs/images/Control_Logic.png)

## Watchdog_Task.c
- Periodic watchdog refresh
- Timeout handling

![Watchdog_Task.c](docs/images/Watchdog_Task.png)

## Comm_Task.c
- Send logs / status through UART or CAN
- Package data for transmission

## Error_Handler.c
- Central error processing
- Recovery attempt
- Safe shutdown if needed

![Error_Handler.c](docs/images/Error_Handler.png)

--END--
