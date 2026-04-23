# Test Module

## Overview
This folder contains validation and test programs for verifying firmware functionality, diagnostics, communication, and state estimation.
It demonstrates how individual modules are tested and validated during development.

---

## Objectives
- Validate communication integrity
- Verify diagnostic logic
- Test safety thresholds
- Validate SOC estimation using AI
---

## Test Structure
```text
tests/
├── test_pec15.c
├── test_spi_comm.c
├── test_gpio_range.c
├── test_openwire.c
├── test_short_check.c
├── test_redundancy.c
├── test_runtime_diagnostics.c
```

## Test Coverage
- CRC/PEC validation
- SPI communication
- Sensor plausibility
- Open-wire detection
- Short-circuit detection
- Redundancy checks
- Runtime diagnostics
- AI-based SOC estimation

## Notes
- Tests can be executed via UART logging or debugger
- Designed for demonstration and validation purposes
├── test_soc_ai.c
├── test_soc_ekf.c
└── test_logs.md
