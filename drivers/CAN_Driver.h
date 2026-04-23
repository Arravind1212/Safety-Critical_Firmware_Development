// can_driver.h

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define CAN_RX_BUFFER_SIZE 64
#define CAN_TX_BUFFER_SIZE 64

// CAN Identifiers for UDS
#define UDS_REQUEST_ID   0x7E0
#define UDS_RESPONSE_ID  0x7E8

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
} CAN_Frame;

void CAN_Init(void);
bool CAN_SendFrame(uint32_t id, const uint8_t *data, uint8_t len);
bool CAN_ReceiveFrame(CAN_Frame *frame);

#endif // CAN_DRIVER_H




