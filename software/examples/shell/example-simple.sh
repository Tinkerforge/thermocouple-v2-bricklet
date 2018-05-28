#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Thermocouple Bricklet 2.0

# Get current temperature
tinkerforge call thermocouple-v2-bricklet $uid get-temperature
