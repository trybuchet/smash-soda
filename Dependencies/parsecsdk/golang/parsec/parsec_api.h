#ifndef PARSEC_API_H
#define PARSEC_API_H

#include <stdint.h>
#include <stdbool.h>

// Function declarations for a minimal set of functions
int32_t ParsecInit_go(uint32_t ver, int32_t upnp, int32_t clientPort, int32_t hostPort, void **parsecInstance);
void ParsecDestroy_go(void *parsecInstance);
uint32_t ParsecVersion_go(void);

#endif // PARSEC_API_H