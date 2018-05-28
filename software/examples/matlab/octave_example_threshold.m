function octave_example_threshold()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Thermocouple Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    t = javaObject("com.tinkerforge.BrickletThermocoupleV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register temperature callback to function cb_temperature
    t.addTemperatureCallback(@cb_temperature);

    % Configure threshold for temperature "greater than 30 °C"
    % with a debounce period of 10s (10000ms)
    t.setTemperatureCallbackConfiguration(10000, false, ">", 30*100, 0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for temperature callback
function cb_temperature(e)
    fprintf("Temperature: %g °C\n", e.temperature/100.0);
end
