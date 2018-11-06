use std::{error::Error, io};

use tinkerforge::{ipconnection::IpConnection, thermocouple_v2_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Thermocouple Bricklet 2.0

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let thermocouple_v2_bricklet = ThermocoupleV2Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Get current temperature
    let temperature = thermocouple_v2_bricklet.get_temperature().recv()?;
    println!("Temperature: {}{}", temperature as f32 / 100.0, " °C");

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
