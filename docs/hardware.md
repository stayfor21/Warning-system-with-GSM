# Hardware Documentation

This document describes the hardware part of the Arduino-based emergency warning system with GSM notification support.

The purpose of this file is to explain which components are used, what each component does, how the components are connected, and what hardware limitations should be considered during real-world testing.

## Hardware Overview

The system is built around an Arduino UNO board. Arduino receives signals from sensors, processes them in the firmware, and controls output devices such as NeoPixel LEDs, a servo motor, buzzer and GSM module.

The hardware part of the system includes:

- environmental sensors;
- motion detection;
- visual warning indicators;
- emergency door mechanism;
- GSM communication module;
- sound alarm;
- power supply elements.

## Main Hardware Components

| Component | Purpose |
|---|---|
| Arduino UNO | Main microcontroller and central control unit |
| MQ-2 gas sensor | Detects gas or smoke concentration |
| TMP36 temperature sensor | Measures ambient temperature |
| IR motion sensor | Detects movement near the emergency exit |
| Photoresistor | Measures ambient light level |
| Servo motor | Opens and closes the emergency door mechanism |
| NeoPixel LEDs | Provide EXIT, FIRE, warning and evacuation path indication |
| SIM800L GSM module | Sends SMS notifications in real hardware mode |
| Buzzer | Provides sound alarm during emergency mode |
| Breadboard | Used for prototyping connections |
| Jumper wires | Used for wiring components |
| External power supply | Recommended for GSM module, LEDs and servo motor |

## Component Roles

## Arduino UNO

Arduino UNO is the main controller of the system.

It performs the following tasks:

- reads analog and digital sensor values;
- processes sensor data;
- detects dangerous conditions;
- switches between operating modes;
- controls NeoPixel LEDs;
- controls the servo motor;
- controls the buzzer;
- communicates with the GSM module;
- outputs diagnostic information to the Serial Monitor.

Arduino UNO was selected because it is simple, widely supported, compatible with the Arduino IDE, and suitable for educational embedded systems projects.

## MQ-2 Gas Sensor

The MQ-2 sensor is used for gas or smoke detection.

In this project, it is connected to an analog input and returns a value from 0 to 1023.

The firmware compares this value with the configured threshold:

`GAS_ALARM = 500`

If the gas value reaches or exceeds this threshold, the system switches to Alarm mode.

The reset threshold is lower:

`GAS_RESET = 430`

This prevents unstable switching when the gas value is close to the alarm threshold.

### Notes

MQ-2 sensors usually require warm-up time and calibration before accurate use. In a real device, calibration should be performed according to the environment and sensor datasheet.

## TMP36 Temperature Sensor

The TMP36 sensor is used to measure temperature.

The sensor is connected to an analog input. The firmware converts the analog value into voltage and then into Celsius.

The current alarm threshold is:

`TEMP_ALARM = 30°C`

The reset threshold is:

`TEMP_RESET = 27°C`

If the temperature reaches or exceeds the alarm threshold, the system enters Alarm mode.

### Notes

TMP36 is suitable for basic educational and prototype-level temperature monitoring. For a more advanced version, the sensor could be replaced with BME280, DS18B20 or another digital temperature sensor.

## IR Motion Sensor

The IR motion sensor detects movement near the emergency exit.

It is connected to a digital input pin.

When motion is detected:

- the system switches to Motion mode;
- the servo opens the emergency door;
- way LEDs turn on;
- the system keeps monitoring gas and temperature.

Motion alone does not trigger SMS alerts because it is not treated as a critical emergency event in this project.

## Photoresistor

The photoresistor measures ambient light level.

It is used to control LED brightness automatically.

The firmware maps the analog light value to a brightness range and prevents the brightness from becoming too low.

This allows EXIT and warning indicators to remain visible in different lighting conditions.

## Servo Motor

The servo motor represents the emergency door mechanism.

The firmware uses three main servo positions:

| Position | Value | Meaning |
|---|---:|---|
| Closed | `0` | Door closed |
| Open | `90` | Door open |
| Warning pulse | `30` | Short warning movement during Alarm mode |

The door opens during:

- Motion mode;
- Alarm mode.

The door closes during:

- Normal mode;
- Fault mode;
- after motion timeout.

### Notes

A real servo motor may require an external power supply. Powering a servo directly from Arduino can cause unstable behavior, resets or insufficient torque.

## NeoPixel LEDs

NeoPixel LEDs are used for visual indication.

The project uses several LED groups:

| LED Group | Purpose |
|---|---|
| Main luster | General lighting and warning brightness |
| Way LEDs | Evacuation path and gas level indication |
| Text line LEDs | EXIT and FIRE visual messages |

NeoPixel LEDs are controlled using the Adafruit NeoPixel library.

### Main Luster

The main luster contains 12 NeoPixel LEDs.

It is used for:

- normal room lighting;
- visual warning in Alarm mode;
- fault indication blinking.

### Way LEDs

The way LEDs contain 4 NeoPixel LEDs.

They are used for:

- evacuation path indication;
- gas level indication in Normal mode;
- moving red emergency indication in Alarm mode.

### Text Line LEDs

The text indication is built from 5 LED lines, each containing 4 NeoPixel LEDs.

These lines are used to display animated EXIT and FIRE patterns.

The patterns are stored in firmware as arrays:

- `TEXT_EXIT`
- `TEXT_FIRE`

## SIM800L GSM Module

The SIM800L GSM module is used to send SMS alerts during emergency situations.

The firmware communicates with the module using SoftwareSerial.

The GSM module is checked using AT commands:

| Command | Purpose |
|---|---|
| `AT` | Checks module response |
| `ATE0` | Disables echo |
| `AT+CMGF=1` | Enables SMS text mode |
| `AT+CPIN?` | Checks SIM card status |
| `AT+CSQ` | Checks signal quality |
| `AT+CREG?` | Checks network registration |

If the module is ready, the firmware can send SMS alerts depending on the alarm reason.

### Important Power Note

SIM800L should not be powered directly from the Arduino 5V pin.

A stable external power supply is recommended because SIM800L can consume high current peaks during network communication.

Recommended hardware considerations:

- use external power supply;
- connect common GND with Arduino;
- use a proper antenna;
- use a SIM card with SMS support;
- ensure sufficient signal quality;
- avoid long unstable jumper wires for power.

## Buzzer

The buzzer is used as an additional sound alarm.

It is activated only during Alarm mode.

The firmware toggles the buzzer on and off at a fixed interval to create an intermittent alarm signal.

The buzzer can be enabled or disabled in firmware using:

`USE_BUZZER`

## Pin Configuration

| Function | Arduino Pin |
|---|---|
| Photoresistor | A1 |
| IR motion sensor | D12 |
| Servo motor | D2 |
| Gas sensor | A3 |
| Temperature sensor | A5 |
| Buzzer | A0 |
| GSM RX | D10 |
| GSM TX | D11 |
| Main NeoPixel luster | D3 |
| Way NeoPixels | D7 |
| Text line 1 | D6 |
| Text line 2 | D5 |
| Text line 3 | D4 |
| Text line 4 | D8 |
| Text line 5 | D9 |

## Connection Summary

## Analog Inputs

| Sensor | Pin |
|---|---|
| Photoresistor | A1 |
| Gas sensor | A3 |
| Temperature sensor | A5 |

## Digital Inputs

| Sensor | Pin |
|---|---|
| IR motion sensor | D12 |

## Digital Outputs

| Device | Pin |
|---|---|
| Servo motor | D2 |
| Main NeoPixel luster | D3 |
| Text line 3 | D4 |
| Text line 2 | D5 |
| Text line 1 | D6 |
| Way NeoPixels | D7 |
| Text line 4 | D8 |
| Text line 5 | D9 |
| Buzzer | A0 |

## Serial Communication

| Module | Arduino Pin |
|---|---|
| GSM RX | D10 |
| GSM TX | D11 |

## Power Recommendations

For simulation, Arduino power is usually enough.

For real hardware, the following components may require external power:

- SIM800L GSM module;
- servo motor;
- NeoPixel LEDs;
- buzzer, depending on type.

Recommended power setup:

| Component | Power Recommendation |
|---|---|
| Arduino UNO | USB or regulated 5V |
| NeoPixel LEDs | External 5V if many LEDs are used |
| Servo motor | External 5V supply recommended |
| SIM800L | External stable supply strongly recommended |
| Sensors | Arduino 5V or 3.3V depending on module |

All grounds must be connected together:

`Arduino GND ↔ Sensor GND ↔ Servo GND ↔ NeoPixel GND ↔ GSM GND`

Without common ground, signals between modules may become unstable or unreadable.

## Current Consumption Considerations

NeoPixel LEDs, servo motors and GSM modules can consume significant current.

Possible issues if power is insufficient:

- Arduino resets;
- servo jitters;
- GSM module fails to register on the network;
- NeoPixels flicker;
- SMS sending fails;
- sensor readings become unstable.

For real testing, it is recommended to power high-current components separately while keeping common ground with Arduino.

## Suggested Wiring Order

When assembling the circuit, it is recommended to connect and test components step by step.

Suggested order:

1. Connect Arduino UNO.
2. Connect Serial Monitor and verify firmware upload.
3. Connect temperature sensor.
4. Connect gas sensor.
5. Connect photoresistor.
6. Connect IR motion sensor.
7. Connect servo motor.
8. Connect NeoPixel LEDs.
9. Connect buzzer.
10. Connect GSM module last.

Testing components one by one makes troubleshooting easier.

## Simulation Notes

The project can be partially tested in Tinkercad.

Tinkercad can be used to test:

- Arduino logic;
- analog sensor simulation;
- digital motion input;
- servo motor movement;
- LED behavior;
- Serial Monitor output.

However, Tinkercad does not fully support SIM800L GSM behavior. Therefore, SMS sending should be simulated in the Serial Monitor or tested on real hardware.

## Real Hardware Notes

For real hardware testing:

- check sensor wiring before powering the system;
- use a stable external supply for SIM800L;
- use a common ground;
- check GSM antenna connection;
- test AT commands before testing the full system;
- do not use maximum NeoPixel brightness for long periods without proper power planning;
- ensure the servo does not overload the power supply;
- keep sensor wires short where possible.

## Hardware Limitations

This project is a prototype and has several hardware limitations:

- MQ-2 requires calibration and warm-up time;
- TMP36 is suitable for basic temperature monitoring but not for certified fire detection;
- SIM800L requires stable power and mobile network coverage;
- Arduino UNO has limited memory and processing power;
- NeoPixel LEDs can consume high current at full brightness;
- breadboard connections can be unreliable for long-term use;
- the system is not certified for real emergency use.

## Possible Hardware Improvements

Future hardware improvements may include:

- replacing Arduino UNO with ESP32;
- adding Wi-Fi support;
- adding Bluetooth configuration;
- adding OLED or LCD display;
- adding SD card for event logging;
- adding real-time clock module;
- adding backup battery;
- adding charging circuit;
- designing a PCB;
- adding enclosure;
- adding more reliable industrial sensors;
- adding relay output for external siren;
- adding multiple phone numbers for SMS alerts;
- adding GPS module for location reporting.

## Hardware Safety Notes

This project is intended for educational, prototype and portfolio purposes.

For real safety-critical use, the system would need:

- certified fire and gas sensors;
- professional power design;
- electrical protection;
- enclosure;
- battery backup;
- stable communication testing;
- compliance with local safety standards;
- professional installation and validation.

## Conclusion

The hardware design demonstrates how Arduino can be combined with sensors, LEDs, servo control and GSM communication to create a prototype emergency warning system.

The current hardware structure is suitable for simulation, education, portfolio demonstration and further development into a real prototype.
