#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletThermocoupleV2;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Thermocouple Bricklet 2.0

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $t = Tinkerforge::BrickletThermocoupleV2->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get current temperature
my $temperature = $t->get_temperature();
print "Temperature: " . $temperature/100.0 . " °C\n";

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
