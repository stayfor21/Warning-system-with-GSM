# Warning System with GSM

Arduino-based emergency warning system designed to detect dangerous situations such as gas leakage, smoke, high temperature, and motion.
The system reacts automatically by activating LED indicators, opening an emergency exit mechanism, displaying system messages, and preparing GSM-based SMS notifications.

## Project Overview

This project was developed as part of a bachelor's thesis in Computer Engineering.
The main goal was to create a hardware-software prototype of an emergency notification system based on Arduino and GSM communication.

The system is designed for environments where fast local reaction is important, such as residential buildings, educational facilities, small industrial areas, or smart home safety systems.

## Key Features

* Gas and smoke detection
* High temperature detection
* Motion detection
* Emergency lighting using NeoPixel LEDs
* Servo-controlled emergency door mechanism
* Adaptive brightness using a photoresistor
* Serial Monitor status output
* GSM/SMS notification logic
* Alarm mode and normal mode behavior
* Tested in Tinkercad simulation environment

## Hardware Components

| Component        | Purpose                                  |
| ---------------- | ---------------------------------------- |
| Arduino UNO      | Main microcontroller                     |
| MQ-2             | Gas and smoke detection                  |
| TMP36            | Temperature measurement                  |
| SIM800L          | GSM/SMS notification module              |
| Photoresistor    | Ambient light detection                  |
| IR motion sensor | Motion detection                         |
| Servo motor      | Emergency door control                   |
| NeoPixel LEDs    | Visual warning and evacuation indication |
| Serial Monitor   | System status output                     |

## How It Works

The system continuously reads data from connected sensors.

In normal mode, the system displays an EXIT indication and monitors the environment.

When motion is detected, the system opens the door and activates evacuation lighting.

When gas, smoke, or high temperature is detected, the system switches to alarm mode:

1. Opens the emergency door
2. Activates warning LEDs
3. Displays emergency indication
4. Outputs system status to the Serial Monitor
5. Sends or prepares an SMS alert through the GSM module

## System Modes

### Normal Mode

The system monitors all sensors and displays the emergency exit indication.

### Motion Mode

The system detects a person near the exit area, opens the door, and highlights the evacuation path.

### Alarm Mode

The system detects dangerous conditions and activates emergency behavior, including visual warning, door control, and GSM alert logic.

## Technologies Used

* Arduino C++
* Arduino UNO
* GSM communication
* AT commands
* Adafruit NeoPixel
* Servo library
* SoftwareSerial
* Tinkercad


## Testing

The system was tested in the Tinkercad simulation environment.
The main tested scenarios included:

* Normal system operation
* Motion detection
* High temperature detection
* Gas/smoke detection
* LED indication behavior
* Servo motor reaction
* Serial Monitor messages

## Limitations

Due to Tinkercad limitations, the GSM module was not fully simulated.
However, the project architecture includes GSM/SMS notification logic and can be extended for real hardware testing with a SIM800L module.

## Future Improvements

* Add real SIM800L hardware testing
* Add Wi-Fi or Bluetooth support
* Add mobile app integration
* Add backup battery power
* Add SD card logging
* Add LCD or OLED display
* Design a PCB version
* Create a 3D-printed enclosure

## Thesis Context

This project was created as a bachelor's thesis focused on the development of an Arduino-based emergency warning system with GSM notification support.
