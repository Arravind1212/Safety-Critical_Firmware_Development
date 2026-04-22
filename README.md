# Safety-Critical_Firmware_Development
RTOS-based Battery Management System firmware with real-time cell monitoring, diagnostics, and safety control.

## Overview
This project demonstrates the development of a real-time embedded firmware architecture for a safety-critical sensor monitoring 
and control system using an RTOS-based design.

- The system is built around a high-performance automotive MCU (NXP32K) and interfaces with external sensing hardware through an isolated communication protocol. 
It performs real-time data acquisition, safety monitoring, diagnostics, and control actions in a deterministic manner.

- The firmware follows a modular layered architecture (Application → Service → HAL) and incorporates robust communication handling, 
fault detection, and safety mechanisms aligned with automotive functional safety principles.

- The firmware is designed as a real-time, safety-critical embedded system running on an RTOS-based architecture.
It interfaces with external sensor and monitoring hardware through a robust isolated communication interface.

## Technical Description
The firmware architecture is divided into three major layers:
 1. Application Layer  
   Implements system-level logic including monitoring, safety evaluation, control decisions, and error handling.
 2. Service Layer  
   Handles protocol-specific operations, command encoding/decoding, CRC validation, and diagnostic routines.
 3. Hardware Abstraction Layer (HAL)  
   Provides low-level drivers for SPI, GPIO, UART, CAN, and timer peripherals, ensuring portability and hardware independence.

The system is designed to be scalable and suitable for automotive applications, with considerations for ISO26262 safety compliance, 
modularity, and deterministic execution.

The system performs the following **key functions**:
- Real-time acquisition of sensor data using ADC-based measurement
- Execution of safety-critical checks including threshold monitoring and fault detection
- Communication with external devices using SPI over an isolated interface
- Data integrity validation using CRC-based error detection (PEC15)
- Implementation of watchdog mechanisms to ensure system reliability
- Execution of startup and runtime diagnostics to detect latent faults

## RTOS Task Design
![RTOS Tasks](docs/images/rtos_tasks.png)
- Monitoring Task → Sensor acquisition + safety checks  
- Diagnostics Task → Startup & runtime validation  
- Watchdog Task → System health supervision  
- Communication Task → Logging / data exchange  

## Communication Design
![Communication Flow](docs/images/communication_flow.png)

- Command + CRC-based communication
- Data integrity validation using CRC
- Reliable communication over isolated interface

## Safety Features
- Startup diagnostics before system operation  
- Runtime fault detection  
- Watchdog-based recovery  
- Open/short detection logic  
- Centralized error handling  
- Safe shutdown mechanism  

## Performance & Optimization
- Low CPU utilization with high headroom  
- Support for non-blocking communication (ISR-based)  
- Optimized task scheduling and prioritization  
- Efficient memory usage strategy  

## Presentation
📄 [View Full Technical Presentation](docs/firmware_architecture.pdf)

## Future Improvements
- Sensor fusion integration  
- State estimation (Kalman Filter)  
- Advanced control algorithms  
- ROS / autonomous system integration  
