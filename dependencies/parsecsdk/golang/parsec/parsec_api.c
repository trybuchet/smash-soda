#include "parsec_api.h"
#include "parsec.h" // Include the original Parsec SDK header

// Implement wrapper functions
int32_t ParsecInit_go(uint32_t ver, int32_t upnp, int32_t clientPort, int32_t hostPort, void **parsecInstance) {
    // Create a temporary ParsecConfig struct
    ParsecConfig cfg = {
        .upnp = upnp,
        .clientPort = clientPort,
        .hostPort = hostPort,
    };
    Parsec *ps;
    ParsecStatus status = ParsecInit(ver, &cfg, NULL, &ps);
    *parsecInstance = (void *)ps;
    return status; // Return the raw status
}

void ParsecDestroy_go(void *parsecInstance) {
    ParsecDestroy((Parsec *)parsecInstance);
}

uint32_t ParsecVersion_go(void) {
    return ParsecVersion();
}