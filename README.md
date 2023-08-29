# ArduinoBBQControl
This is a hobby project for controlling a gas grill using a servo motor for long jobs (Spare ribs, pulled pork, beef brisket).


# Mechanics

The mechanics is built using a MG995 servo motor and a 3D printed "TriPod" chassis (https://www.thingiverse.com/thing:2511398).

# LCD

An Arduino compatible Hitachi HD44780 LCD is used for displaying information.
Only pins D4 - D7 + RS + EN are used.

# HMI

A push button and a potentiometer slider is used for selecting servo offsets and target temperature.
An additional potentiometer slider can be used to simulate the temperature in the grill chamber for debugging purposes.

# Temperature sensor

IKEA Fantast (new version) temperature sensor is supported by using a voltage divider (default: 47kOhm).
More sensors will be supported soon.

# Configuration

All configuration options are available in config.hpp
Pin assignments are configurable based on the actual wiring.

# Control Algorithm
On first starup, the software will request for a new configuration to be set.
This includes servo offsets and target temperature.
The configuration is stored within EEPROM of compatible Arduiuno boards.
If a reset occurs, the software will prompt, if a new configuration shall be set.
Otherwise, the last configuration will be loaded.
After finishing configuration, the software will drive the servo motor from the configured minimum to maximum servo offset.
In a next step, the software switches into "Heatup" state with maximum gas flow, until the configured target temperature is reached.
After that, normal temperature control will start and the software will increase or decrease the gas flow based on actual vs. target temperature.
In case the lid of the grill is opened, the software will keep the current actuator state for the time defined in the config file and resume normal operation afterwards.
The hardware watchdog is used to detect a stuck program and will reset the Arduino in case the watchdog is not reset within the configured time window.

# Exmaple circuit diagram

will follow soon
