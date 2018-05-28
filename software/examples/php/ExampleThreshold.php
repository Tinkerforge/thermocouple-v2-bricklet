<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletThermocoupleV2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletThermocoupleV2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Thermocouple Bricklet 2.0

// Callback function for temperature callback
function cb_temperature($temperature)
{
    echo "Temperature: " . $temperature/100.0 . " °C\n";
}

$ipcon = new IPConnection(); // Create IP connection
$t = new BrickletThermocoupleV2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register temperature callback to function cb_temperature
$t->registerCallback(BrickletThermocoupleV2::CALLBACK_TEMPERATURE, 'cb_temperature');

// Configure threshold for temperature "greater than 30 °C"
// with a debounce period of 10s (10000ms)
$t->setTemperatureCallbackConfiguration(10000, FALSE, '>', 30*100, 0);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
