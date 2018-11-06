use std::{error::Error, io, thread};
use tinkerforge::{ipconnection::IpConnection, thermocouple_v2_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Thermocouple Bricklet 2.0

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let thermocouple_v2_bricklet = ThermocoupleV2Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    //Create listener for temperature events.
    let temperature_listener = thermocouple_v2_bricklet.get_temperature_receiver();
    // Spawn thread to handle received events. This thread ends when the thermocouple_v2_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in temperature_listener {
            println!("Temperature: {}{}", event as f32 / 100.0, " °C");
        }
    });

    // Configure threshold for temperature "greater than 30 °C"
    // with a debounce period of 10s (10000ms)
    thermocouple_v2_bricklet.set_temperature_callback_configuration(10000, false, '>', 30 * 100, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
