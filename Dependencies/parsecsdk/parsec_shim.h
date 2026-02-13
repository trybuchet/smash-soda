#ifndef PARSEC_SHIM_H
#define PARSEC_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Define a handle for the Parsec instance
typedef void* ParsecSDKInstance;

// Function to initialize the Parsec SDK
// Returns 0 on success, non-zero on error
int32_t ParsecShimInit(uint32_t version, int32_t upnp, int32_t clientPort, int32_t hostPort, ParsecSDKInstance* instance);

// Function to destroy the Parsec SDK instance
void ParsecShimDestroy(ParsecSDKInstance instance);

// Function to get the Parsec SDK version
uint32_t ParsecShimGetVersion();

#ifdef __cplusplus
}
#endif

#endif // PARSEC_SHIM_H