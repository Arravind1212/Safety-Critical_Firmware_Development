# Configuration Module

## Overview
This folder contains system wide configuration parameters used across all layers of the embedded firmware.
It centralizes constants such as thresholds, task timing, communication settings, and hardware mappings to ensure maintainability and scalability.

---

## Objectives
- Centralize system configuration
- Avoid hardcoding values in source files
- Enable easy tuning and calibration
- Support scalability across different platforms
---

## Configuration Files
```text
configs/
├── system_config.h
├── safety_thresholds.h
├── task_config.h
├── comm_config.h
├── pin_config.h
├── diag_config.h
└── ltc6813_config.h
```

## System Configuration
General system behavior and feature enable flags.

## Safety Thresholds
Defines voltage, temperature, and fault detection limits.

## Task Configuration
Defines RTOS task periods, priorities, and scheduling parameters.

## Communication Configuration
SPI, UART, CAN, ISO-TP, and UDS settings.

## Pin Configuration
Hardware pin mappings for MCU interfaces.

## Diagnostics Configuration
Diagnostic enable flags and timing parameters.

## Device Configuration
Device-specific constants and command settings.

--END--
