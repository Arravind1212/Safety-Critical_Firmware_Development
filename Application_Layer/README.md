## Application_Layer
Firmware application layer contains the following main system behavior.

<p align="left"> <img src="../docs/images/Application_Layer_Outline.png" width="400"/> </p>

## main.c
- Program entry
- Initialize MCU/peripherals
- Create RTOS tasks
- Start scheduler

<p align="left"> <img src="../docs/images/main.png" width="600"/> </p>

## System_Init.c
- Startup sequence
- Wake external device
- Clear registers
- Load default configuration
- Call startup diagnostics

<p align="left"> <img src="../docs/images/System_Init.png" width="600"/> </p>

## Sensor_Monitoring.c
- Periodic data read
- Convert raw ADC values
- Threshold checks
- Trigger control actions
- Control decisions based on measurements
※ For example threshold-based control, actuator enable/disable, balancing-like logic

<p align="left"> <img src="../docs/images/Control_Logic.png" width="600"/> </p>

## Watchdog_Task.c
- Periodic watchdog refresh
- Timeout handling

<p align="left"> <img src="../docs/images/Watchdog_Task.png" width="600"/> </p>

## Comm_Task.c
- Send logs / status through UART or CAN
- Package data for transmission

## Error_Handler.c
- Central error processing
- Recovery attempt
- Safe shutdown if needed

<p align="left"> <img src="../docs/images/Error_Handler.png" width="600"/> </p>

--END--
