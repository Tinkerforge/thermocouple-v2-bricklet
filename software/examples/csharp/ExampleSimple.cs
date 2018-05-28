using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Thermocouple Bricklet 2.0

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletThermocoupleV2 t = new BrickletThermocoupleV2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current temperature
		int temperature = t.GetTemperature();
		Console.WriteLine("Temperature: " + temperature/100.0 + " °C");

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
