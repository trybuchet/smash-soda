#include "parsec_wrapper.h"
#include "parsec.h"

ParsecStatus ParsecInit_wrapper(uint32_t ver, const ParsecConfig *cfg, const void *reserved, Parsec **ps) {
    return ParsecInit(ver, cfg, reserved, ps);
}

uint32_t ParsecVersion_wrapper(void) {
    return ParsecVersion();
}