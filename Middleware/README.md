# Control / Service Layer

## Overview
- This layer implements the device-oriented control and protocol logic of the embedded system.
- It acts as an interface between the low-level drivers and the application layer, 
handling command generation, communication sequencing, CRC/PEC validation, and device-specific operations.
- The module is designed to isolate protocol complexity from application logic and provide reusable APIs for monitoring, diagnostics, and control.

---

## Objectives
- Abstract device-specific communication logic from the application layer
- Build and transmit protocol commands through the SPI driver
- Validate data integrity using PEC15 / CRC
- Provide high-level APIs for configuration, acquisition, and control
- Support scalable and maintainable embedded software architecture

---

## Layer Position

<p align="center">  <img src="../docs/images/Firmware_Architecture.png" width="500"/></p>

This layer sits between:
- **Drivers Layer** → SPI, GPIO, UART, CAN, Timer  
- **Application Layer** → Monitoring, diagnostics, watchdog, error handling  

---

## Main Responsibilities
- Device wake-up sequence
- Register clear and configuration write
- ADC conversion trigger
- Readback of measured data
- Command encoding / decoding
- PEC15 generation and validation
- Support for diagnostics and periodic monitoring

---

## Module Structure

```text
control/
├── README.md
├── ltc6813_driver.c
├── ltc6813_driver.h
├── pec15.c
└── pec15.h
