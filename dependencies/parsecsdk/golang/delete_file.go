package main

import (
	"fmt"
	"os"
)

func main() {
	filePath := `C:\Users\User\Documents\Smash Soda\Smash Soda\dependencies\parsecsdk\golang\parsec.go`
	err := os.Remove(filePath)
	if err != nil {
		fmt.Printf("Error deleting file: %v\n", err)
		os.Exit(1)
	}
	fmt.Println("File deleted successfully.")
}