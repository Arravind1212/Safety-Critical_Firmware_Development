#include "gpio_driver.h"
#include "S32K344.h"

#define TEMP1_PORT  PTB
#define TEMP1_PIN   5  // PTB5

#define TEMP2_PORT  PTB
#define TEMP2_PIN   6  // PTB6

void GPIO_InitTempSensors(void) {
    // Enable clock for PORTB
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;

    // Set PTB5 and PTB6 as GPIO input
    PORTB->PCR[TEMP1_PIN] = PORT_PCR_MUX(1);
    PORTB->PCR[TEMP2_PIN] = PORT_PCR_MUX(1);

    TEMP1_PORT->PDDR &= ~(1 << TEMP1_PIN);  // Input
    TEMP2_PORT->PDDR &= ~(1 << TEMP2_PIN);  // Input
}

int Read_TempSensor1(void) {
    return (TEMP1_PORT->PDIR & (1 << TEMP1_PIN)) ? 1 : 0;
}

int Read_TempSensor2(void) {
    return (TEMP2_PORT->PDIR & (1 << TEMP2_PIN)) ? 1 : 0;
}