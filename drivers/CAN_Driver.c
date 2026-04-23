/can_driver.c

#include "can_driver.h"
#include "S32K344.h"
#include "FreeRTOS.h"
#include "queue.h"

static QueueHandle_t canRxQueue = NULL;

void CAN0_ORed_0_15_MB_IRQHandler(void) {
    if (CAN0->IFLAG1 & (1 << 4)) {
        CAN_Frame frame;
        frame.id = (CAN0->RAMn[4 * 4 + 1] >> 18);
        frame.dlc = (CAN0->RAMn[4 * 4 + 0] >> 16) & 0x0F;

        uint32_t data = CAN0->RAMn[4 * 4 + 2];
        for (int i = 0; i < frame.dlc; i++) {
            frame.data[i] = (data >> (8 * (3 - i))) & 0xFF;
        }

        xQueueSendFromISR(canRxQueue, &frame, NULL);
        CAN0->IFLAG1 = (1 << 4); // Clear flag
    }
}

void CAN_Init(void) {
    PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;

    CAN0->MCR |= CAN_MCR_MDIS_MASK;
    CAN0->MCR &= ~CAN_MCR_MDIS_MASK;
    while (!(CAN0->MCR & CAN_MCR_FRZACK_MASK));

    CAN0->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;
    CAN0->CTRL1 = 0x04DB0006; // 500kbps

    CAN0->RAMn[4 * 4 + 1] = 0x00080000 | (UDS_REQUEST_ID << 18);
    CAN0->RAMn[4 * 4 + 0] = 0x04000000;

    CAN0->RXMGMASK = 0x1FFFFFFF;
    CAN0->IMASK1 |= (1 << 4); // Enable interrupt for MB4

    CAN0->MCR &= ~CAN_MCR_HALT_MASK;
    while (CAN0->MCR & CAN_MCR_FRZACK_MASK);

    canRxQueue = xQueueCreate(CAN_RX_BUFFER_SIZE, sizeof(CAN_Frame));
    NVIC_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
}

bool CAN_SendNonBlocking(const CAN_Frame *frame) {
    uint8_t mb = 0;

    CAN0->RAMn[mb * 4 + 1] = (frame->id << 18);
    CAN0->RAMn[mb * 4 + 2] = 0;
    CAN0->RAMn[mb * 4 + 3] = 0;

    for (int i = 0; i < frame->dlc; i++) {
        CAN0->RAMn[mb * 4 + 2] |= frame->data[i] << (8 * (3 - i));
    }

    CAN0->RAMn[mb * 4 + 0] = (0xC4000000 | (frame->dlc << 16));
    return true;
}

bool CAN_ReceiveFromQueue(CAN_Frame *frame, TickType_t timeout) {
    return xQueueReceive(canRxQueue, frame, timeout);
}
