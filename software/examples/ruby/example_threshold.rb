#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_thermocouple_v2'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Thermocouple Bricklet 2.0

ipcon = IPConnection.new # Create IP connection
t = BrickletThermocoupleV2.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Register temperature callback
t.register_callback(BrickletThermocoupleV2::CALLBACK_TEMPERATURE) do |temperature|
  puts "Temperature: #{temperature/100.0} °C"
end

# Configure threshold for temperature "greater than 30 °C"
# with a debounce period of 10s (10000ms)
t.set_temperature_callback_configuration 10000, false, '>', 30*100, 0

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
