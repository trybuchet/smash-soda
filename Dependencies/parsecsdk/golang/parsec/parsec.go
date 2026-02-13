package parsec

/*
#cgo CFLAGS: -IC:/Users/User/Documents/Smash Soda/Smash Soda/dependencies/parsecsdk
#cgo LDFLAGS: -LC:/Users/User/Documents/Smash Soda/Smash Soda/dependencies/parsecsdk/windows -lparsec

#include <stdint.h>
#include <stdbool.h>
#include "parsec.h"

// Wrapper functions
ParsecStatus ParsecInit_go(uint32_t ver, int32_t upnp, int32_t clientPort, int32_t hostPort, Parsec **ps) {
    ParsecConfig cfg = {
        .upnp = upnp,
        .clientPort = clientPort,
        .hostPort = hostPort,
    };
    return ParsecInit(ver, &cfg, NULL, ps);
}

void ParsecDestroy_go(Parsec *ps) {
    ParsecDestroy(ps);
}

uint32_t ParsecVersion_go(void) {
    return ParsecVersion();
}
*/
import (
	"C"
	"unsafe"
)

type ParsecService struct {
	parsecInstance *C.Parsec
}

type ParsecStatus int32

const (
	PARSEC_OK ParsecStatus = 0
)

type ParsecConfig struct {
	Upnp       int32
	ClientPort int32
	HostPort   int32
}

func (s *ParsecService) Init(cfg *ParsecConfig) ParsecStatus {
	cVer := (C.uint32_t)(6<<16 | 0) // Manually define PARSEC_VER
	
	upnp := (C.int32_t)(0)
	clientPort := (C.int32_t)(0)
	hostPort := (C.int32_t)(0)

	if cfg != nil {
		upnp = (C.int32_t)(cfg.Upnp)
		clientPort = (C.int32_t)(cfg.ClientPort)
		hostPort = (C.int32_t)(cfg.HostPort)
	}

	status := ParsecStatus(C.ParsecInit_go(cVer, upnp, clientPort, hostPort, &s.parsecInstance))
	return status
}

func (s *ParsecService) Destroy() {
	C.ParsecDestroy_go(s.parsecInstance)
}

func ParsecVersion() uint32 {
	return uint32(C.ParsecVersion_go())
}