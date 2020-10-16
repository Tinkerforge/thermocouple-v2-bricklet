// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_thermocouple_v2.h"

#define UID "XYZ" // Change XYZ to the UID of your Thermocouple Bricklet 2.0

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


static TF_ThermocoupleV2 t;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_thermocouple_v2_create(&t, UID, hal), "create device object");

	// Get current temperature
	int32_t temperature;
	check(tf_thermocouple_v2_get_temperature(&t, &temperature), "get temperature");

	tf_hal_printf("Temperature: %d 1/%d °C\n", temperature, 100);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
