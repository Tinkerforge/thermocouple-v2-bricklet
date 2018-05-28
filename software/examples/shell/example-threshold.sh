#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Thermocouple Bricklet 2.0

# Handle incoming temperature callbacks
tinkerforge dispatch thermocouple-v2-bricklet $uid temperature &

# Configure threshold for temperature "greater than 30 °C"
# with a debounce period of 10s (10000ms)
tinkerforge call thermocouple-v2-bricklet $uid set-temperature-callback-configuration 10000 false threshold-option-greater 3000 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
