function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletThermocoupleV2;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Thermocouple Bricklet 2.0

    ipcon = IPConnection(); % Create IP connection
    t = handle(BrickletThermocoupleV2(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register temperature callback to function cb_temperature
    set(t, 'TemperatureCallback', @(h, e) cb_temperature(e));

    % Set period for temperature callback to 1s (1000ms) without a threshold
    t.setTemperatureCallbackConfiguration(1000, false, 'x', 0, 0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for temperature callback
function cb_temperature(e)
    fprintf('Temperature: %g °C\n', e.temperature/100.0);
end
