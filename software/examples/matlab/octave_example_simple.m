function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Thermocouple Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    t = javaObject("com.tinkerforge.BrickletThermocoupleV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current temperature
    temperature = t.getTemperature();
    fprintf("Temperature: %g °C\n", temperature/100.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
