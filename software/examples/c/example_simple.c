#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_thermocouple_v2.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change XYZ to the UID of your Thermocouple Bricklet 2.0

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	ThermocoupleV2 t;
	thermocouple_v2_create(&t, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Get current temperature
	int32_t temperature;
	if(thermocouple_v2_get_temperature(&t, &temperature) < 0) {
		fprintf(stderr, "Could not get temperature, probably timeout\n");
		return 1;
	}

	printf("Temperature: %f °C\n", temperature/100.0);

	printf("Press key to exit\n");
	getchar();
	thermocouple_v2_destroy(&t);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
