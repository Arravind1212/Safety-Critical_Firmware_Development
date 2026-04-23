#ifndef LTC6813_DRIVER_H
#define LTC6813_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_CELLS        18
#define NUM_CELL_GROUPS  6

void LTC6813_Wake(void);
void LTC6813_ClearAllRegisters(void);
bool LTC6813_WriteDefaultConfig(void);
bool LTC6813_ReadConfig(uint8_t *rx_config);

bool LTC6813_EnableCellDischarge(uint8_t cell_number);
bool LTC6813_DisableCellDischarge(uint8_t cell_number);

void LTC6813_StartCellADC(void);
void LTC6813_PollADCComplete(void);
bool LTC6813_ReadCellGroup(uint8_t group, uint16_t *voltages_mv);
bool LTC6813_ReadAllCellVoltages(uint16_t *all_cells_mv);

void LTC6813_StartAUXADC(void);
bool LTC6813_ReadAuxGroup(uint8_t group, uint16_t *aux_data);

bool LTC6813_WriteSControl(uint8_t *scontrol_data);

#endif // LTC6813_DRIVER_H