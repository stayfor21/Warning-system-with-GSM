# Firmware Architecture

This document describes the internal firmware architecture of the Arduino-based emergency warning system with GSM notification support.

The purpose of this document is to explain how the code is organized, how the system states work, how sensor values are processed, and how the firmware reacts to emergency conditions.

## Firmware Location

The main firmware file is located at:

`src/main.cpp`

The firmware is written in Arduino C++ and is designed for Arduino UNO or compatible boards.

## Main Design Idea

The firmware is based on a state-driven architecture.

Instead of placing all logic directly inside the `loop()` function, the system is divided into operating states. Each state has its own behavior and transition rules.

This makes the firmware easier to:

- understand;
- test;
- extend;
- debug;
- maintain;
- present as a portfolio project.

## Main System States

The firmware uses the following system states:

| State | Purpose |
|---|---|
| `Normal` | Default monitoring mode |
| `Motion` | Motion detected near the emergency exit |
| `Alarm` | Dangerous gas/smoke or temperature condition detected |
| `Fault` | Invalid sensor values or sensor error condition detected |

These states are defined using an enum:

`enum class SystemState : uint8_t`

Using an enum makes the code more readable and safer than using plain numbers or string values.

## State Machine Overview

The firmware works as a finite-state machine.

Basic state transitions:

| Current State | Condition | Next State |
|---|---|---|
| `Normal` | Motion detected | `Motion` |
| `Normal` | Gas or temperature alarm | `Alarm` |
| `Normal` | Sensor error | `Fault` |
| `Motion` | Gas or temperature alarm | `Alarm` |
| `Motion` | Motion timeout expired | `Normal` |
| `Motion` | Sensor error | `Fault` |
| `Alarm` | Safe values are stable for reset time | `Normal` |
| `Alarm` | Sensor error | `Fault` |
| `Fault` | Sensor values become valid again | `Normal` |

## Main Loop Structure

The `loop()` function performs three main tasks:

1. Reads sensors at a fixed interval.
2. Prints system status at a fixed interval.
3. Calls the handler for the current system state.

The main loop does not contain all system logic directly. Instead, it delegates behavior to state handlers.

Main state handlers:

| Function | Purpose |
|---|---|
| `handleNormal()` | Handles Normal mode behavior |
| `handleMotion()` | Handles Motion mode behavior |
| `handleAlarm()` | Handles Alarm mode behavior |
| `handleFault()` | Handles Fault mode behavior |

This separation makes the code cleaner and easier to modify.

## Sensor Data Structure

Sensor values are stored in a structure:

`struct SensorData`

The structure contains:

| Field | Meaning |
|---|---|
| `temp` | Current filtered temperature value |
| `gas` | Current filtered gas/smoke sensor value |
| `light` | Current filtered ambient light value |
| `motion` | Current motion sensor state |

Using a structure keeps all sensor data grouped in one logical object.

## Sensor Reading

Sensor data is updated by the `readSensors()` function.

This function reads:

- temperature sensor;
- gas sensor;
- photoresistor;
- IR motion sensor.

The firmware uses separate raw reading functions:

| Function | Purpose |
|---|---|
| `readTemperatureRaw()` | Reads and converts TMP36 analog value to Celsius |
| `readGasRaw()` | Reads gas sensor analog value |
| `readLightRaw()` | Reads photoresistor value and maps it to brightness |
| `readMotionRaw()` | Reads digital IR motion sensor state |

## Sensor Filtering

The firmware does not use raw sensor values directly.

Instead, it applies simple smoothing using the function:

`smoothValue()`

This helps reduce sudden jumps caused by sensor noise.

Filtered values are used for:

- temperature alarm detection;
- gas alarm detection;
- adaptive LED brightness;
- system diagnostics.

This improves system stability and reduces false triggering.

## Alarm Detection

Alarm detection is based on temperature and gas values.

The firmware uses the function:

`alarmCondition()`

The alarm is triggered if at least one of the following conditions is true:

- temperature is greater than or equal to `TEMP_ALARM`;
- gas value is greater than or equal to `GAS_ALARM`.

## Alarm Reasons

The firmware can detect different alarm reasons.

These are defined using:

`enum class AlarmReason : uint8_t`

Possible alarm reasons:

| Reason | Meaning |
|---|---|
| `None` | No alarm condition |
| `Gas` | Gas or smoke value exceeded threshold |
| `Temperature` | Temperature exceeded threshold |
| `GasAndTemperature` | Both gas and temperature exceeded thresholds |

The function responsible for detecting the alarm reason is:

`getAlarmReason()`

This allows the firmware to send different SMS messages depending on the detected danger.

## Alarm Thresholds

The firmware uses separate alarm and reset thresholds.

| Constant | Value | Purpose |
|---|---:|---|
| `TEMP_ALARM` | 30 | Temperature value that activates Alarm mode |
| `TEMP_RESET` | 27 | Temperature value required for reset |
| `GAS_ALARM` | 500 | Gas value that activates Alarm mode |
| `GAS_RESET` | 430 | Gas value required for reset |

The reset thresholds are lower than alarm thresholds. This creates hysteresis.

## Hysteresis Logic

Hysteresis prevents unstable switching between Alarm and Normal modes.

Without hysteresis, the system could rapidly switch between states if the sensor value stays close to the alarm threshold.

Example:

- Alarm starts at 30°C.
- Alarm does not reset immediately at 29°C.
- The temperature must fall to 27°C or lower.
- The safe condition must remain stable for a configured time.

This makes the system more reliable.

## Safe Condition Logic

The firmware uses the function:

`safeCondition()`

The system is considered safe only when:

- temperature is less than or equal to `TEMP_RESET`;
- gas value is less than or equal to `GAS_RESET`.

After this condition becomes true, the firmware waits for:

`ALARM_STABLE_RESET_TIME`

Only after this delay the system returns to Normal mode.

## Normal Mode Behavior

Normal mode is handled by:

`handleNormal()`

In Normal mode, the firmware:

- updates the main luster LEDs;
- displays EXIT indication;
- displays gas level using way LEDs;
- monitors all sensors;
- keeps the door closed;
- waits for motion or alarm conditions.

Possible transitions from Normal mode:

| Condition | Transition |
|---|---|
| Motion detected | `Motion` |
| Gas or temperature alarm detected | `Alarm` |
| Sensor error detected | `Fault` |

## Motion Mode Behavior

Motion mode is handled by:

`handleMotion()`

In Motion mode, the firmware:

- opens the servo-controlled door;
- turns on way LEDs;
- continues reading all sensors;
- does not send SMS because motion alone is not treated as an emergency alarm.

If motion stops, the firmware waits for:

`MOTION_HOLD_TIME`

After this timeout, the door closes and the system returns to Normal mode.

If gas or high temperature is detected during Motion mode, the system immediately switches to Alarm mode.

## Alarm Mode Behavior

Alarm mode is handled by:

`handleAlarm()`

In Alarm mode, the firmware:

- opens the emergency door;
- keeps the door open;
- activates FIRE animation;
- activates moving red way indication;
- sets luster LEDs to maximum brightness;
- activates buzzer alarm if enabled;
- sends or simulates SMS alerts;
- prints alarm reason to Serial Monitor.

The system remains in Alarm mode until safe conditions are stable for the configured reset time.

## Fault Mode Behavior

Fault mode is handled by:

`handleFault()`

Fault mode is used when sensor values are outside the expected operating range.

In Fault mode, the firmware:

- closes the door;
- stops the buzzer;
- disables normal alarm behavior;
- blinks luster LEDs as a fault indication;
- waits for sensor values to become valid again.

When the sensor values return to the valid range, the system returns to Normal mode.

## Door Control

The servo motor represents the emergency door mechanism.

Door control functions:

| Function | Purpose |
|---|---|
| `openDoor()` | Opens the door if it is not already open |
| `closeDoor()` | Closes the door if it is currently open |
| `forceCloseDoor()` | Forces the door to the closed position |

The firmware stores the door state using:

`doorIsOpen`

This prevents unnecessary repeated servo commands.

## LED Architecture

The project uses several NeoPixel groups.

| LED Group | Purpose |
|---|---|
| Luster LEDs | Main room or warning lighting |
| Way LEDs | Evacuation path and gas level indication |
| Text line LEDs | EXIT and FIRE text indication |

The firmware stores text-line LED strips in an array:

`Adafruit_NeoPixel *lines[LINE_COUNT]`

This allows the code to update all text rows in loops instead of writing repeated code for each strip.

## EXIT Indication

EXIT indication is displayed in Normal mode.

The pattern is stored in:

`TEXT_EXIT`

The firmware uses:

`animateExit()`

and:

`showExitFrame()`

to display and animate the EXIT text using the NeoPixel text lines.

## FIRE Indication

FIRE indication is displayed in Alarm mode.

The pattern is stored in:

`TEXT_FIRE`

The firmware uses:

`animateFire()`

and:

`showFireFrame()`

to create a moving FIRE animation.

This provides a visual emergency warning when gas, smoke or high temperature is detected.

## Adaptive Brightness

The firmware uses the photoresistor to control LED brightness.

The function:

`adaptiveBrightness()`

returns a safe brightness value based on the ambient light sensor.

The function:

`updateLuster()`

uses this value to adjust the main lighting.

The system prevents brightness from dropping below:

`MIN_BRIGHTNESS`

This keeps indicators visible even in low-light conditions.

## Gas Level Indicator

In Normal mode, the way LEDs can also show the current gas level.

The function:

`showGasLevel()`

maps the gas sensor value to the number of active way LEDs.

This gives a visual indication of increasing gas level before the alarm threshold is reached.

## Buzzer Logic

The buzzer is controlled by:

`updateAlarmBuzzer()`

The buzzer toggles during Alarm mode to create an audible warning.

The buzzer can be enabled or disabled with:

`USE_BUZZER`

If disabled, the firmware will not activate sound output.

## GSM Configuration

GSM behavior can be enabled or disabled with:

`USE_GSM`

If GSM is enabled, the firmware communicates with the GSM module using SoftwareSerial.

If GSM is disabled, SMS messages are printed to the Serial Monitor instead of being sent.

This is useful for simulation and debugging.

## GSM Initialization

The GSM module is initialized by:

`initGsm()`

The firmware checks the module using:

`checkGsm()`

The module is tested with several AT commands:

| Command | Purpose |
|---|---|
| `AT` | Checks if the module responds |
| `ATE0` | Disables command echo |
| `AT+CMGF=1` | Enables SMS text mode |
| `AT+CPIN?` | Checks SIM card status |
| `AT+CSQ` | Checks signal quality |
| `AT+CREG?` | Checks network registration |

If the module responds correctly, the firmware sets:

`gsmReady = true`

If the module does not respond, the system can still continue local emergency behavior without SMS sending.

## SMS Logic

SMS messages are sent by:

`sendSms()`

The SMS text depends on the alarm reason.

Gas or smoke:

`Увага! Виявлено дим або газ у приміщенні.`

High temperature:

`Увага! Температура перевищила безпечний рівень.`

Gas and high temperature:

`Увага! Виявлено дим або газ і високу температуру.`

## SMS Anti-Spam Protection

The firmware includes SMS anti-spam protection.

The constant:

`SMS_REPEAT_TIME`

defines how often repeated SMS alerts can be sent during a long alarm.

This prevents the system from sending too many messages while the same emergency condition continues.

## Timing System

The firmware uses `millis()`-based timing for most periodic tasks.

Main timing constants:

| Constant | Purpose |
|---|---|
| `SENSOR_INTERVAL` | Sensor reading interval |
| `STATUS_INTERVAL` | Serial status printing interval |
| `EXIT_INTERVAL` | EXIT animation update interval |
| `GAS_INTERVAL` | Gas level indicator update interval |
| `MOTION_HOLD_TIME` | Door hold time after motion stops |
| `ALARM_STABLE_RESET_TIME` | Safe period before leaving Alarm mode |
| `SMS_REPEAT_TIME` | Delay between repeated SMS messages |
| `FIRE_FRAME_INTERVAL` | FIRE animation frame interval |
| `FAULT_BLINK_INTERVAL` | Fault LED blinking interval |
| `BUZZER_INTERVAL` | Buzzer toggle interval |

Using `millis()` allows the firmware to perform several tasks without fully blocking the main loop.

Some short delays are still used for servo pulse and GSM communication, but the main logic is mostly interval-based.

## Serial Diagnostics

The firmware prints system status to the Serial Monitor.

The main diagnostic function is:

`printStatus()`

It prints:

- current system state;
- alarm reason;
- temperature value;
- gas value;
- light value;
- motion sensor state;
- GSM readiness.

Example output:

`State: NORMAL | Reason: NONE | Temp: 24 | Gas: 250 | Light: 120 | IR: 0 | GSM: READY`

This helps during debugging, testing and demonstration.

## Memory Optimization

The firmware stores large text pattern arrays in program memory using:

`PROGMEM`

This is important for Arduino UNO because it has limited SRAM.

The arrays:

- `TEXT_FIRE`
- `TEXT_EXIT`

are stored in flash memory instead of RAM.

This improves memory usage and makes the firmware more suitable for Arduino UNO.

## Main Advantages of the Current Firmware

The current firmware has several improvements compared to a simple prototype:

- state-based architecture;
- separated state handlers;
- sensor smoothing;
- alarm hysteresis;
- stable alarm reset logic;
- SMS anti-spam protection;
- GSM readiness checking;
- simulation mode for GSM;
- buzzer support;
- fault mode;
- adaptive brightness;
- animated EXIT and FIRE indication;
- Serial Monitor diagnostics;
- reduced RAM usage with PROGMEM.

## Possible Future Refactoring

The current project uses a single `main.cpp` file.

For a larger version, the code could be split into several files:

| File | Purpose |
|---|---|
| `config.h` | Thresholds, intervals and feature flags |
| `pins.h` | Arduino pin configuration |
| `text_patterns.h` | EXIT and FIRE LED patterns |
| `sensors.h / sensors.cpp` | Sensor reading and filtering |
| `display.h / display.cpp` | NeoPixel display logic |
| `gsm.h / gsm.cpp` | GSM and SMS logic |
| `door.h / door.cpp` | Servo door control |
| `state_machine.h / state_machine.cpp` | System state logic |

This would make the firmware easier to scale, but for the current portfolio version, keeping the code in one file is acceptable and easier to review.

## Conclusion

The firmware is structured as a compact embedded system with clear operating states and automatic emergency response logic.

It demonstrates:

- sensor-based monitoring;
- state-machine programming;
- emergency event handling;
- servo control;
- NeoPixel visual feedback;
- GSM/SMS communication logic;
- Serial Monitor diagnostics;
- embedded system safety improvements.

This architecture makes the project suitable for a portfolio as an Arduino, IoT and embedded systems prototype.
