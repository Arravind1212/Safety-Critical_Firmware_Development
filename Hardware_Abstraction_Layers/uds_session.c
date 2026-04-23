// UDS_session.c

#include "uds_session.h"

// Configuration
#define SESSION_TIMEOUT_SEC 5  // Session timeout (in seconds)

// Static state
static UDS_SessionType currentSession = UDS_SESSION_DEFAULT;
static uint8_t sessionTimer = 0;

void UDS_Session_Init(void) {
    currentSession = UDS_SESSION_DEFAULT;
    sessionTimer = 0;
}

void UDS_Session_Set(UDS_SessionType session) {
    currentSession = session;

    // Reset timer only if not default session
    if (session != UDS_SESSION_DEFAULT) {
        sessionTimer = SESSION_TIMEOUT_SEC;
    }
}

UDS_SessionType UDS_Session_Get(void) {
    return currentSession;
}

void UDS_Session_Tick(void) {
    if (currentSession != UDS_SESSION_DEFAULT && sessionTimer > 0) {
        sessionTimer--;
        if (sessionTimer == 0) {
            currentSession = UDS_SESSION_DEFAULT;
        }
    }
}

bool UDS_Session_IsActive(void) {
    return (currentSession != UDS_SESSION_DEFAULT);
}
