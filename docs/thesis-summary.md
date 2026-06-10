# Thesis Summary

This document provides a short summary of the bachelor's thesis related to this repository.

The purpose of this file is to explain the academic background of the project and show how the current GitHub version is connected with the original thesis work.

## Thesis Title

Notification System for Emergency Situations Based on a GSM Module

Original Ukrainian title:

Система сповіщення щодо потрапляння в надзвичайну ситуацію на базі GSM модуля

## Academic Context

This project was developed as a bachelor's qualification work in the field of Computer Engineering.

The work focused on designing and testing a hardware-software system for detecting emergency situations and automatically reacting to them using Arduino-based logic and GSM notification support.

## Main Idea

The main idea of the thesis was to create a prototype of an emergency warning system that can detect dangerous conditions and react without direct human control.

The system is designed to detect:

* smoke or gas leakage;
* high temperature;
* motion near an emergency exit.

After detecting a dangerous situation, the system should automatically activate warning behavior, open the emergency door mechanism, display system messages and support GSM/SMS notification logic.

## Problem Relevance

Emergency situations require fast detection and fast information delivery.

Traditional warning systems can have several limitations:

* dependence on local infrastructure;
* limited coverage;
* delayed notification;
* lack of internet connection in some areas;
* outdated hardware;
* insufficient automation.

The thesis considered GSM communication as a practical solution because GSM-based SMS notification can work without Wi-Fi or local internet access. This makes GSM useful for warning systems in remote areas, buildings, industrial environments and smart home safety prototypes.

## Research Object

The research object of the thesis was the process of warning people about emergency and dangerous situations.

## Research Subject

The research subject was a hardware-software system using a GSM module for automatic emergency notification.

## Thesis Goal

The goal of the thesis was to develop and test an Arduino-based emergency warning system that can automatically detect dangerous factors and react according to a predefined scenario.

The system had to detect:

* fire-related conditions;
* gas leakage;
* high temperature;
* motion.

The system also had to react by:

* activating visual indicators;
* opening the emergency door mechanism;
* displaying diagnostic messages in the Serial Monitor;
* supporting GSM/SMS notification functionality.

## Main Thesis Tasks

The main tasks of the thesis were:

1. Analyze existing methods of emergency notification.
2. Analyze existing public warning and emergency alert systems.
3. Select and justify the hardware components for the system.
4. Develop Arduino C++ firmware for the prototype.
5. Implement status output through the Serial Monitor.
6. Test the system using main emergency scenarios.

## Selected Components

The system was designed around accessible and Arduino-compatible components.

| Component        | Purpose                                                                    |
| ---------------- | -------------------------------------------------------------------------- |
| Arduino UNO      | Main controller for processing sensor data and controlling system behavior |
| MQ-2             | Gas and smoke detection                                                    |
| TMP36            | Temperature measurement                                                    |
| SIM800L          | GSM/SMS notification support                                               |
| Photoresistor    | Ambient light detection and adaptive brightness                            |
| IR motion sensor | Motion detection near the emergency exit                                   |
| Servo motor      | Emergency door control                                                     |
| NeoPixel LEDs    | Visual EXIT, FIRE and warning indication                                   |
| Serial Monitor   | System diagnostics and status output                                       |

## Component Selection Reasoning

The components were selected based on availability, simplicity, compatibility with Arduino and suitability for simulation or prototype development.

Arduino UNO was chosen as the central controller because it is simple, widely supported and suitable for educational embedded systems projects.

MQ-2 was selected for gas and smoke detection because it is affordable and commonly used in Arduino-based safety prototypes.

TMP36 was selected for temperature monitoring because it provides analog temperature output and can be used easily with Arduino.

SIM800L was selected as the GSM module because it is compact, affordable and widely used in Arduino projects for SMS communication.

The photoresistor was included to support automatic brightness adjustment for LED indication.

NeoPixel LEDs were used for visual indication, including EXIT and FIRE patterns.

## System Functionality

The thesis prototype implemented the following main functions:

* reading sensor values;
* detecting dangerous conditions;
* reacting to gas or smoke;
* reacting to high temperature;
* reacting to motion;
* opening the emergency door mechanism;
* controlling LED indication;
* outputting status messages to the Serial Monitor;
* preparing GSM/SMS notification functionality.

## Operating Logic

The system continuously monitors sensor values.

In normal conditions, the system displays EXIT indication and remains in monitoring mode.

When motion is detected, the system opens the emergency door and activates evacuation path indication.

When gas, smoke or high temperature is detected, the system enters alarm behavior:

* the door opens;
* warning indication starts;
* FIRE visual indication is displayed;
* system status is printed to the Serial Monitor;
* SMS alert logic is activated or prepared.

## Testing Environment

The original prototype was tested in the Tinkercad simulation environment.

Tinkercad was used to verify:

* sensor behavior;
* Arduino logic;
* servo motor reaction;
* LED indication;
* Serial Monitor output;
* main emergency scenarios.

However, full GSM/SMS testing requires real hardware because Tinkercad does not fully simulate SIM800L network registration and real mobile SMS communication.

## Testing Results

The system was tested using the main operating scenarios:

| Scenario                  | Expected System Reaction                       |
| ------------------------- | ---------------------------------------------- |
| Normal conditions         | EXIT indication and sensor monitoring          |
| Motion detected           | Door opens and way LEDs activate               |
| High temperature detected | Alarm mode starts                              |
| Gas or smoke detected     | Alarm mode starts                              |
| Safe values restored      | System returns to normal behavior              |
| Status monitoring         | Sensor and state data appear in Serial Monitor |

The tests confirmed that the system logic can detect simulated dangerous situations and activate the required response.

## Limitations of the Thesis Prototype

The original thesis prototype had several limitations:

* GSM/SMS communication was not fully tested in real mobile network conditions;
* the prototype was mainly tested in simulation;
* real sensor calibration was not fully covered;
* power supply stability for SIM800L, servo and LEDs requires real hardware validation;
* no PCB or enclosure was designed;
* the system was not certified for real safety-critical use.

## GitHub Version Improvements

The current GitHub version improves the original prototype and makes it more suitable for portfolio presentation.

Main improvements include:

* cleaner firmware structure;
* state-based system logic;
* separate operating states: Normal, Motion, Alarm and Fault;
* sensor value filtering;
* alarm hysteresis;
* stable alarm reset logic;
* GSM readiness checking;
* SMS anti-spam logic;
* buzzer support;
* fault mode;
* animated EXIT indication;
* animated FIRE indication;
* improved Serial Monitor diagnostics;
* detailed documentation;
* structured repository layout.

## Difference Between Thesis Prototype and Repository Version

The thesis prototype focused on proving the concept and testing the main behavior in simulation.

The repository version focuses on presenting the project as a more complete embedded systems portfolio project.

| Aspect         | Thesis Prototype                       | GitHub Version                                              |
| -------------- | -------------------------------------- | ----------------------------------------------------------- |
| Main goal      | Academic prototype and simulation      | Portfolio-ready embedded project                            |
| Code structure | Basic Arduino firmware                 | Refactored state-based firmware                             |
| Testing        | Tinkercad scenarios                    | Documented test cases and limitations                       |
| GSM            | Planned or partially implemented logic | Improved GSM command and SMS logic                          |
| Documentation  | Thesis document                        | README, user guide, testing, hardware and architecture docs |
| Presentation   | Academic work                          | GitHub portfolio project                                    |

## Practical Use Cases

The system concept can be applied in:

* smart home safety systems;
* small building emergency warning prototypes;
* educational embedded systems projects;
* Arduino-based IoT demonstrations;
* gas and smoke monitoring prototypes;
* evacuation indication systems;
* industrial safety concept demonstrations.

## Future Development

Possible future improvements include:

* real SIM800L hardware testing;
* adding Wi-Fi support with ESP32 or ESP8266;
* adding Bluetooth configuration;
* adding OLED or LCD display;
* adding SD card logging;
* adding event timestamps;
* adding backup battery power;
* designing a PCB;
* creating a 3D-printed enclosure;
* adding mobile app integration;
* adding multiple phone numbers for alerts;
* adding GPS location reporting;
* improving sensor calibration.

## Conclusion

The thesis demonstrated that an Arduino-based system can be used as a prototype for detecting emergency situations and reacting automatically.

The system combines sensors, visual indication, servo control, Serial Monitor diagnostics and GSM/SMS notification logic.

The current repository presents an improved version of the original thesis project and shows the project as a practical embedded systems and IoT portfolio example.
