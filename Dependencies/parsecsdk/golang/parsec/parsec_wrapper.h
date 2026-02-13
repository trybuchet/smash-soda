#ifndef PARSEC_WRAPPER_H
#define PARSEC_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

// Forward declarations for types used in function signatures
typedef struct Parsec Parsec;
typedef int32_t ParsecStatus;
typedef struct ParsecConfig ParsecConfig;

// Function declarations
ParsecStatus ParsecInit_wrapper(uint32_t ver, const ParsecConfig *cfg, const void *reserved, Parsec **ps);
uint32_t ParsecVersion_wrapper(void);

#endif // PARSEC_WRAPPER_H