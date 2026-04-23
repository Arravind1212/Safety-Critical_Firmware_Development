// uds_session.h

#ifndef UDS_SESSION_H
#define UDS_SESSION_H

#include <stdint.h>
#include <stdbool.h>

// UDS Session Types
typedef enum {
    UDS_SESSION_DEFAULT = 0x01,
    UDS_SESSION_PROGRAMMING = 0x02,
    UDS_SESSION_EXTENDED = 0x03
} UDS_SessionType;

// UDS Session Management
void UDS_Session_Init(void);
void UDS_Session_Set(UDS_SessionType session);
UDS_SessionType UDS_Session_Get(void);
void UDS_Session_Tick(void);  // Call this every 1 second
bool UDS_Session_IsActive(void);

#endif // UDS_SESSION_H
