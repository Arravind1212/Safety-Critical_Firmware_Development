// UDS security.c

#include "uds_security.h"
#include <stdbool.h>
#include <stdint.h>

static uint8_t currentSeed = 0;
static bool securityUnlocked = false;

void UDS_Security_Init(void) {
    // Initialize security state at ECU startup
    securityUnlocked = false;
}

bool UDS_Security_IsUnlocked(void) {
    return securityUnlocked;
}

void UDS_Security_Lock(void) {
    securityUnlocked = false;
}

// === Dynamic Seed Generation ===
static uint8_t generate_dynamic_seed(void) {
    // Example: Simple changing seed using incrementing counter
    // For production: use hardware RNG or entropy source
    static uint8_t seedCounter = 0x5A;
    seedCounter += 0x13;  // Vary seed to prevent fixed patterns
    return seedCounter;
}

// === Tester Seed Request ===
uint8_t UDS_Security_GetSeed(void) {
    currentSeed = generate_dynamic_seed();
    return currentSeed;
}

// === Tester Sends Key → Validate ===
bool UDS_Security_ValidateKey(uint8_t key) {
    uint8_t expectedKey = currentSeed ^ 0xFF;  // Inverse of seed
    if (key == expectedKey) {
        securityUnlocked = true;
        return true;
    }
    return false;
}
