// UDS service.c

#include "uds_service.h"
#include "uds_security.h"
#include "ltc6813_driver.h"
#include "soc_estimator.h"
#include <string.h>

#define UDS_SID_SESSION_CONTROL   0x10
#define UDS_SID_SECURITY_ACCESS   0x27
#define UDS_SID_READ_DATA_BY_ID   0x22

#define UDS_NEG_RESP              0x7F
#define UDS_POS_RESP_OFFSET       0x40

#define PID_CELL1_VOLTAGE         0xF100
#define PID_SOC_ESTIMATE          0xF101

static uint8_t sessionMode = 0x01; // Default session

void UDS_Service_Init(void) {
    UDS_Security_Init();
}

// === Dispatcher Entry Point ===
void UDS_Service_DispatchRequest(const uint8_t* req, 
    uint8_t len, uint8_t* resp, uint8_t* respLen) {
    uint8_t sid = req[0];

    switch (sid) {
        case UDS_SID_SESSION_CONTROL:
            if (len < 2) goto send_nrc;
            sessionMode = req[1];
            resp[0] = sid + UDS_POS_RESP_OFFSET;
            resp[1] = sessionMode;
            *respLen = 2;
            return;

        case UDS_SID_SECURITY_ACCESS:
            if (len < 2) goto send_nrc;
            if (req[1] == 0x01) {
                uint8_t seed = UDS_Security_GetSeed();
                resp[0] = sid + UDS_POS_RESP_OFFSET;
                resp[1] = 0x01;
                resp[2] = seed;
                *respLen = 3;
                return;
            } else if (req[1] == 0x02 && len >= 3) {
                bool valid = UDS_Security_ValidateKey(req[2]);
                if (valid) {
                    resp[0] = sid + UDS_POS_RESP_OFFSET;
                    resp[1] = 0x02;
                    *respLen = 2;
                    return;
                } else {
                    goto send_nrc;
                }
            }
            goto send_nrc;

        case UDS_SID_READ_DATA_BY_ID:
            if (!UDS_Security_IsUnlocked()) goto send_nrc;
            if (len < 3) goto send_nrc;
            {
                uint16_t pid = (req[1] << 8) | req[2];
                resp[0] = sid + UDS_POS_RESP_OFFSET;
                resp[1] = req[1];
                resp[2] = req[2];

                switch (pid) {
                    case PID_CELL1_VOLTAGE: {
                        float v = LTC6813_GetCellVoltage(0);
                        uint16_t mv = (uint16_t)(v * 1000);
                        resp[3] = (mv >> 8) & 0xFF;
                        resp[4] = mv & 0xFF;
                        *respLen = 5;
                        return;
                    }
                    case PID_SOC_ESTIMATE: {
                        uint8_t soc = SOC_Estimate();
                        resp[3] = soc;
                        *respLen = 4;
                        return;
                    }
                    default: goto send_nrc;
                }
            }

        default:
        send_nrc:
            resp[0] = UDS_NEG_RESP;
            resp[1] = sid;
            resp[2] = 0x13; // Incorrect format or conditions not met
            *respLen = 3;
    }
}
