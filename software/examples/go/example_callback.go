package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
	"github.com/Tinkerforge/go-api-bindings/thermocouple_v2_bricklet"
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

	t.RegisterTemperatureCallback(func(temperature int32) {
		fmt.Printf("Temperature: %f °C\n", float64(temperature)/100.0)
	})

	// Set period for temperature callback to 1s (1000ms) without a threshold.
	t.SetTemperatureCallbackConfiguration(1000, false, 'x', 0, 0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
