#include "parsec_shim.h"
#include "parsec.h" // Original Parsec SDK header

// Function to initialize the Parsec SDK
int32_t ParsecShimInit(uint32_t version, int32_t upnp, int32_t clientPort, int32_t hostPort, ParsecSDKInstance* instance) {
    ParsecConfig config = {
        .upnp = upnp,
        .clientPort = clientPort,
        .hostPort = hostPort,
    };
    Parsec* parsecInstance = nullptr;
    ParsecStatus status = ParsecInit(version, &config, nullptr, &parsecInstance);
    *instance = static_cast<ParsecSDKInstance>(parsecInstance);
    return status;
}

// Function to destroy the Parsec SDK instance
void ParsecShimDestroy(ParsecSDKInstance instance) {
    ParsecDestroy(static_cast<Parsec*>(instance));
}

// Function to get the Parsec SDK version
uint32_t ParsecShimGetVersion() {
    return ParsecVersion();
}