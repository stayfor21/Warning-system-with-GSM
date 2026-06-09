# User Guide

This document explains how to set up, upload, run and use the Arduino-based emergency warning system with GSM notification support.

## Project Purpose

The system is designed to detect potentially dangerous situations such as gas leakage, smoke, high temperature and motion near an emergency exit.

When a dangerous condition is detected, the system automatically reacts by:

- opening the emergency door using a servo motor;
- activating NeoPixel visual warning indicators;
- displaying system status in the Serial Monitor;
- sending or preparing an SMS alert through the GSM module;
- activating the buzzer during alarm mode.

## Required Components

| Component | Purpose |
|---|---|
| Arduino UNO | Main controller |
| MQ-2 gas sensor | Gas or smoke detection |
| TMP36 temperature sensor | Temperature measurement |
| IR motion sensor | Motion detection |
| Photoresistor | Ambient light detection |
| Servo motor | Emergency door control |
| NeoPixel LEDs | EXIT and alarm indication |
| SIM800L GSM module | SMS notification |
| Buzzer | Sound alarm |
| Jumper wires | Connections |
| Breadboard | Prototyping |
| External power supply | Stable power for GSM, LEDs and servo |

## Required Libraries

Install the following Arduino libraries:

- Adafruit NeoPixel
- Servo
- SoftwareSerial

Servo and SoftwareSerial are usually included with the Arduino IDE.

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

## Before Uploading

Open the firmware file:

`src/main.cpp`

Find this line:

`const char PHONE[] = "+380XXXXXXXXX";`

Replace the placeholder phone number with your own number.

Also check these options:

`constexpr bool USE_GSM = true;`

`constexpr bool USE_BUZZER = true;`

If you are testing without a GSM module, set:

`constexpr bool USE_GSM = false;`

In this mode, SMS messages will be printed to the Serial Monitor instead of being sent through GSM.

## Uploading the Firmware

1. Open Arduino IDE.
2. Open `src/main.cpp`.
3. Select the correct board: `Tools → Board → Arduino UNO`.
4. Select the correct port: `Tools → Port`.
5. Install required libraries.
6. Click Upload.

## Serial Monitor

After uploading the firmware, open the Serial Monitor.

Use baud rate:

`115200`

The system should print information similar to:

`[GSM] Checking module`

`[GSM] Module ready`

`[SYSTEM] Started`

`State: NORMAL | Reason: NONE | Temp: 24 | Gas: 250 | Light: 120 | IR: 0 | GSM: READY`

If the GSM module is not connected or not ready, the system may print:

`[GSM] Module not ready`

The system can still work locally without GSM.

## Operating Modes

## Normal Mode

This is the default mode.

In this mode:

- sensors are monitored;
- EXIT indication is displayed;
- luster LEDs are active;
- gas level can be shown on the way LEDs;
- door remains closed;
- buzzer is off.

Expected Serial Monitor output:

`State: NORMAL | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: ... | GSM: ...`

## Motion Mode

Motion mode is activated when the IR sensor detects movement.

In this mode:

- the emergency door opens;
- way LEDs turn on;
- system status changes to `MOTION`;
- SMS is not sent because motion alone is not treated as a critical emergency.

Expected Serial Monitor output:

`[STATE] MOTION`

`State: MOTION | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: 1 | GSM: ...`

After motion stops, the system waits for a short timeout and returns to Normal mode.

## Alarm Mode

Alarm mode is activated when temperature reaches or exceeds 30°C, or when the gas value reaches or exceeds 500.

In this mode:

- the emergency door opens;
- FIRE animation starts;
- warning LEDs activate;
- buzzer starts;
- GSM/SMS alert is sent or simulated;
- Serial Monitor displays the alarm reason.

Possible alarm reasons:

- `GAS`
- `TEMPERATURE`
- `GAS_AND_TEMPERATURE`

Expected Serial Monitor output:

`[STATE] ALARM | Reason: GAS`

`[GSM] Sending SMS`

`[GSM] SMS sent`

or:

`[STATE] ALARM | Reason: TEMPERATURE`

## Fault Mode

Fault mode is activated when sensor values are outside expected limits.

In this mode:

- the door closes;
- warning behavior stops;
- luster LEDs blink as a fault indication;
- system waits until sensor values return to a valid range.

Expected Serial Monitor output:

`[STATE] FAULT`

`State: FAULT | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: ... | GSM: ...`

## Alarm Reset Logic

The system does not immediately return to Normal mode after an alarm.

To reset from Alarm mode:

- temperature must be less than or equal to 27°C;
- gas value must be less than or equal to 430.

These safe values must remain stable for 10000 ms.

This prevents unstable switching between Alarm and Normal modes.

## GSM Notes

The GSM module is used to send SMS alerts.

The firmware checks the module using the following AT commands:

- `AT`
- `ATE0`
- `AT+CMGF=1`
- `AT+CPIN?`
- `AT+CSQ`
- `AT+CREG?`

For real hardware:

- use a SIM card with SMS support;
- connect an antenna;
- use a stable external power supply;
- connect common GND between Arduino and GSM module;
- do not power SIM800L directly from Arduino 5V pin.

## SMS Messages

The firmware can send different SMS messages depending on the alarm reason.

Gas or smoke:

`Увага! Виявлено дим або газ у приміщенні.`

High temperature:

`Увага! Температура перевищила безпечний рівень.`

Gas and high temperature:

`Увага! Виявлено дим або газ і високу температуру.`

## Troubleshooting

| Problem | Possible Cause | Solution |
|---|---|---|
| No Serial Monitor output | Wrong baud rate | Set baud rate to 115200 |
| Servo does not move | Wrong pin or weak power | Check D2 and power supply |
| NeoPixels do not light up | Wrong pin or missing library | Check pins and install Adafruit NeoPixel |
| GSM not ready | SIM card, power or antenna issue | Check SIM800L wiring and power |
| SMS not sent | No network or incorrect phone number | Check phone number and signal |
| Alarm triggers too often | Sensor noise or wrong threshold | Adjust threshold values |
| Door closes too fast | Motion timeout too short | Increase `MOTION_HOLD_TIME` |
| LEDs too bright | Brightness settings too high | Adjust brightness constants |

## Safety Notes

This project is a prototype created for educational and portfolio purposes.

For real safety-critical use, the system would require:

- certified sensors;
- reliable power backup;
- enclosure;
- PCB design;
- hardware protection;
- real GSM testing;
- fail-safe logic;
- compliance with safety standards.

## Quick Start

1. Connect all components according to the circuit diagram.
2. Upload `src/main.cpp` to Arduino UNO.
3. Open Serial Monitor at `115200`.
4. Check that the system enters Normal mode.
5. Trigger motion sensor and verify door opening.
6. Increase gas or temperature value and verify Alarm mode.
7. Check SMS behavior if GSM is connected.
