function matlab_example_threshold()
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

    % Configure threshold for temperature "greater than 30 °C"
    % with a debounce period of 10s (10000ms)
    t.setTemperatureCallbackConfiguration(10000, false, '>', 30*100, 0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for temperature callback
function cb_temperature(e)
    fprintf('Temperature: %g °C\n', e.temperature/100.0);
end
