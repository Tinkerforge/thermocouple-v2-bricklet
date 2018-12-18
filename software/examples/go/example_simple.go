package main

import (
	"fmt"
	"github.com/tinkerforge/go-api-bindings/ipconnection"
	"github.com/tinkerforge/go-api-bindings/thermocouple_v2_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Thermocouple Bricklet 2.0.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	t, _ := thermocouple_v2_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Get current temperature.
	temperature, _ := t.GetTemperature()
	fmt.Printf("Temperature: %f °C\n", float64(temperature)/100.0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
