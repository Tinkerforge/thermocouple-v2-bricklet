#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletThermocoupleV2;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Thermocouple Bricklet 2.0

# Callback subroutine for temperature callback
sub cb_temperature
{
    my ($temperature) = @_;

    print "Temperature: " . $temperature/100.0 . " °C\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $t = Tinkerforge::BrickletThermocoupleV2->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register temperature callback to subroutine cb_temperature
$t->register_callback($t->CALLBACK_TEMPERATURE, 'cb_temperature');

# Set period for temperature callback to 1s (1000ms) without a threshold
$t->set_temperature_callback_configuration(1000, 0, 'x', 0, 0);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
