using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Thermocouple Bricklet 2.0

	// Callback function for temperature callback
	static void TemperatureCB(BrickletThermocoupleV2 sender, int temperature)
	{
		Console.WriteLine("Temperature: " + temperature/100.0 + " °C");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletThermocoupleV2 t = new BrickletThermocoupleV2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Register temperature callback to function TemperatureCB
		t.TemperatureCallback += TemperatureCB;

		// Set period for temperature callback to 1s (1000ms) without a threshold
		t.SetTemperatureCallbackConfiguration(1000, false, 'x', 0, 0);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
