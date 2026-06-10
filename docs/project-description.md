# Project Description

This document provides a concise portfolio-oriented description of the Arduino-based emergency warning system with GSM notification support.

Unlike the thesis summary, this file focuses on the project as a practical embedded systems prototype: what problem it solves, how it works, what was implemented, and what engineering value it demonstrates.

## Project Idea

Warning System with GSM is an Arduino-based hardware-software prototype designed to detect potentially dangerous situations and react automatically.

The system monitors environmental and motion-related conditions using several sensors. When a dangerous condition is detected, it activates visual warning indicators, opens an emergency door mechanism, prints diagnostic information to the Serial Monitor, and supports GSM/SMS notification logic.

The project combines sensor-based monitoring, embedded firmware, visual indication, servo control and GSM communication into one integrated prototype.

## Problem

Emergency situations such as gas leakage, smoke, fire-related temperature rise or abnormal movement near an exit require fast detection and immediate response.

Simple warning systems may depend on manual reaction, local sound alarms or internet-based communication. These approaches can be limited in situations where:

- internet connection is unavailable;
- fast local reaction is required;
- users need automatic notification;
- the system must react without human intervention;
- visual and mechanical warning behavior is needed.

This project explores how Arduino and GSM-based logic can be used to build a compact and affordable emergency warning prototype.

## Proposed Solution

The proposed solution is an Arduino-based system that continuously reads sensor values and reacts depending on the detected condition.

The system uses:

- a gas/smoke sensor to detect dangerous air conditions;
- a temperature sensor to detect high temperature;
- a motion sensor to detect movement near the emergency exit;
- a photoresistor to adjust LED brightness;
- NeoPixel LEDs for EXIT, FIRE and warning indication;
- a servo motor to simulate emergency door opening;
- GSM/SMS logic for remote emergency notification;
- Serial Monitor output for diagnostics and testing.

## Main Functionality

The system provides the following main functionality:

- continuous sensor monitoring;
- gas and smoke detection;
- high temperature detection;
- motion detection;
- automatic door opening using a servo motor;
- emergency visual indication using NeoPixel LEDs;
- adaptive LED brightness based on ambient light;
- Serial Monitor diagnostic output;
- GSM/SMS alert logic;
- buzzer alarm support;
- alarm reset after stable safe conditions;
- fault detection for invalid sensor values.

## Operating Modes

The firmware is organized around several operating modes.

| Mode | Purpose |
|---|---|
| `Normal` | Default monitoring mode with EXIT indication |
| `Motion` | Door opens when motion is detected near the exit |
| `Alarm` | Emergency behavior is activated when gas/smoke or high temperature is detected |
| `Fault` | System indicates invalid sensor values or sensor-related problems |

This state-based approach makes the firmware easier to understand, test and extend.

## System Behavior

In normal mode, the system monitors all sensors and displays EXIT indication.

If motion is detected, the system opens the emergency door and activates the evacuation path indication.

If gas, smoke or high temperature is detected, the system switches to alarm mode. In this mode, it opens the door, activates FIRE indication, turns on warning LEDs, starts buzzer behavior if enabled, prints diagnostic data and prepares or sends an SMS alert.

The alarm remains active until sensor values return to safe levels and remain stable for a configured reset period.

## Engineering Approach

The project was implemented using Arduino C++ and follows a structured embedded systems approach.

Key engineering decisions include:

- state-machine architecture;
- separate alarm and reset thresholds;
- sensor value smoothing;
- GSM readiness checking;
- SMS anti-spam protection;
- use of `millis()` timing for periodic tasks;
- use of `PROGMEM` for LED text patterns;
- visual separation of normal, motion, alarm and fault behavior.

These decisions make the project more reliable than a simple linear Arduino sketch.

## Testing

The prototype was tested in the Tinkercad simulation environment.

The tested scenarios included:

- normal operation;
- motion detection;
- high temperature detection;
- gas/smoke alarm;
- combined gas and temperature condition;
- servo reaction;
- LED indication;
- Serial Monitor diagnostics.

The repository also includes visual testing evidence from the Serial Monitor output during a gas/smoke alarm test.

## Repository Version

The GitHub version is an improved and refactored version of the original thesis prototype.

Compared to the initial academic prototype, the repository version includes:

- cleaner code structure;
- state-based firmware logic;
- improved alarm processing;
- GSM/SMS logic improvements;
- buzzer support;
- fault mode;
- better Serial Monitor diagnostics;
- documentation for usage, testing, hardware and architecture;
- visual materials from the thesis project;
- README optimized for portfolio presentation.

## Practical Value

This project demonstrates practical skills in:

- embedded systems development;
- Arduino firmware programming;
- sensor integration;
- GSM/SMS communication logic;
- servo motor control;
- NeoPixel LED indication;
- state-machine design;
- hardware-software integration;
- technical documentation;
- prototype testing and validation.

## Possible Applications

The system concept can be used as a prototype for:

- smart home safety systems;
- gas leakage warning systems;
- small building emergency prototypes;
- educational Arduino projects;
- embedded systems demonstrations;
- IoT safety concepts;
- evacuation indication prototypes.

## Limitations

The project is a prototype and is not certified for real safety-critical use.

Current limitations include:

- GSM/SMS sending requires real SIM800L hardware testing;
- Tinkercad cannot fully simulate real GSM network behavior;
- sensors require calibration for real-world use;
- high-current components require stable external power;
- no PCB version is currently provided;
- no enclosure is currently designed.

## Result

The project successfully demonstrates the concept of an Arduino-based emergency warning system that can detect dangerous conditions and react automatically.

The current repository presents the project as a complete portfolio-ready embedded systems prototype with firmware, documentation, diagrams, testing evidence and thesis context.
