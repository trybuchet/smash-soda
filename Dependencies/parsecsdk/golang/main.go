package main

import (
	"fmt"
	"parsec-sdk-go/parsec"
	"unsafe"
)

func main() {
	fmt.Println("Initializing Parsec SDK...")

	// Create a new ParsecService instance
	service := &parsec.ParsecService{}

	// Initialize Parsec SDK
	status := service.Init(nil)
	if status != parsec.PARSEC_OK {
		fmt.Printf("Failed to initialize Parsec SDK: %d\n", status)
		return
	}
	fmt.Println("Parsec SDK initialized successfully.")

	// Get Parsec SDK version
	version := parsec.ParsecVersion()
	fmt.Printf("Parsec SDK Version: %d (Major: %d, Minor: %d)\n", version, version>>16, version&0xFFFF)

	// Get Parsec Config
	config, status := service.GetConfig()
	if status == parsec.PARSEC_OK {
		fmt.Printf("Parsec Config: %+v\n", config)
	} else {
		fmt.Printf("Failed to get Parsec Config: %d\n", status)
	}

	// Get Outputs
	outputs, numOutputs := parsec.ParsecGetOutputs()
	fmt.Printf("Number of outputs: %d\n", numOutputs)
	for i, output := range outputs {
		fmt.Printf("Output %d: %+v\n", i, output)
	}

	// Get Decoders
	decoders, numDecoders := parsec.ParsecGetDecoders()
	fmt.Printf("Number of decoders: %d\n", numDecoders)
	for i, decoder := range decoders {
		fmt.Printf("Decoder %d: %+v\n", i, decoder)
	}

	// Destroy Parsec SDK instance
	service.Destroy()
	fmt.Println("Parsec SDK destroyed.")
}