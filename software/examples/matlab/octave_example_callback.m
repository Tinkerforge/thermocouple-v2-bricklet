function octave_example_callback()
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

    % Set period for temperature callback to 1s (1000ms) without a threshold
    t.setTemperatureCallbackConfiguration(1000, false, "x", 0, 0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for temperature callback
function cb_temperature(e)
    fprintf("Temperature: %g °C\n", e.temperature/100.0);
end
