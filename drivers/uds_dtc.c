// UDS DTC.c

#include "uds_dtc.h"
#include <string.h>

static DTC_Entry dtcBuffer[MAX_DTC_COUNT];
static uint8_t dtcCount = 0;

void UDS_DTC_Init(void) {
    memset(dtcBuffer, 0, sizeof(dtcBuffer));
    dtcCount = 0;
}

bool UDS_DTC_Add(uint32_t dtcCode, uint8_t status) {
    if (dtcCount >= MAX_DTC_COUNT) return false;

    dtcBuffer[dtcCount].dtcCode = dtcCode;
    dtcBuffer[dtcCount].status = status;
    dtcCount++;
    return true;
}

bool UDS_DTC_Get(uint8_t index, DTC_Entry* outDtc) {
    if (index >= dtcCount) return false;
    *outDtc = dtcBuffer[index];
    return true;
}

bool UDS_DTC_ClearAll(void) {
    memset(dtcBuffer, 0, sizeof(dtcBuffer));
    dtcCount = 0;
    return true;
}

uint8_t UDS_DTC_Count(void) {
    return dtcCount;
}
