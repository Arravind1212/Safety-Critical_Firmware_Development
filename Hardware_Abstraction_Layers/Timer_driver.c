#include "timer_driver.h"
#include "S32K344.h"
#include "cell_monitor.h"

#define LPIT_CHANNEL 0
#define LPIT_FREQ_HZ 1      // 1 Hz timer

void Timer_Init(void) {
    // Enable LPIT clock
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

    // Enable interrupt in NVIC
    NVIC_EnableIRQ(LPIT0_IRQn);

    // Reset and configure timer
    LPIT0->MCR = LPIT_MCR_M_CEN_MASK;           // Enable module
    LPIT0->MIER |= (1 << LPIT_CHANNEL);         // Interrupt enable for CH0
    LPIT0->TVAL[LPIT_CHANNEL] = 80000000;       // 80 MHz / 1Hz
    LPIT0->TCTRL[LPIT_CHANNEL] = LPIT_TCTRL_T_EN_MASK; // Start timer
}

void Timer_StartPeriodicInterrupt(void) {
    LPIT0->TVAL[LPIT_CHANNEL] = 80000000; // Reload again (1s)
    LPIT0->TCTRL[LPIT_CHANNEL] |= LPIT_TCTRL_T_EN_MASK;
}

void LPIT0_IRQHandler(void) {
    if (LPIT0->MSR & (1 << LPIT_CHANNEL)) {
        LPIT0->MSR |= (1 << LPIT_CHANNEL); // Clear interrupt flag
        CellMonitor_TimerCallback();       // ✅ Trigger monitoring function
    }
}