# Testing

This document describes the testing process for the Arduino-based emergency warning system with GSM notification support.

The purpose of testing is to verify that the system correctly reads sensor values, detects dangerous situations, switches between operating modes, activates visual indicators, controls the servo-based emergency door mechanism, and performs GSM/SMS notification logic.

## Tested System

The tested firmware is located in:

```text
src/main.cpp
```

The system is based on Arduino C++ and uses the following main components:

| Component                 | Purpose                                 |
| ------------------------- | --------------------------------------- |
| Arduino UNO               | Main microcontroller                    |
| MQ-2 or analog gas sensor | Gas/smoke detection                     |
| TMP36 temperature sensor  | Temperature monitoring                  |
| IR motion sensor          | Motion detection                        |
| Photoresistor             | Ambient light measurement               |
| NeoPixel LEDs             | EXIT, warning and evacuation indication |
| Servo motor               | Emergency door control                  |
| GSM module / SIM800L      | SMS notification support                |
| Buzzer                    | Additional alarm signal                 |
| Serial Monitor            | Debugging and system status output      |

## Firmware Features Covered by Testing

The tests cover the following firmware features:

* Sensor reading
* Temperature conversion
* Gas level monitoring
* Motion detection
* Light-based adaptive brightness
* Normal mode
* Motion mode
* Alarm mode
* Fault mode
* Servo door control
* NeoPixel visual indication
* FIRE animation
* EXIT animation
* Gas level indicator
* GSM initialization
* SMS alert logic
* SMS anti-spam delay
* Alarm reset logic
* Serial Monitor diagnostics

## System Modes

The firmware uses a state-based logic model.

| State    | Description                                                                                                                                                         |
| -------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Normal` | Default monitoring mode. The system displays EXIT indication and monitors all sensors.                                                                              |
| `Motion` | Activated when the IR sensor detects movement. The door opens and the evacuation path LEDs turn on.                                                                 |
| `Alarm`  | Activated when gas/smoke or high temperature is detected. The system opens the door, activates warning LEDs, starts alarm indication and sends/prepares SMS alerts. |
| `Fault`  | Activated when sensor values are outside expected operating limits. The system shows a fault indication.                                                            |

## Test Environment

Testing can be performed in two environments:

### 1. Tinkercad Simulation

Tinkercad can be used to test:

* Sensor logic
* Temperature threshold behavior
* Gas threshold behavior
* Motion detection
* Servo movement
* NeoPixel indication
* Serial Monitor output

However, Tinkercad does not fully support real GSM/SIM800L communication. Because of this, GSM/SMS behavior should be tested either in simulation mode or on real hardware.

### 2. Real Hardware

Real hardware testing is required to fully verify:

* SIM800L initialization
* SIM card detection
* GSM network registration
* SMS sending
* Real sensor behavior
* Power stability
* Servo movement under real load
* LED brightness and current consumption

## Serial Monitor Settings

Use the following Serial Monitor settings:

| Parameter   | Value    |
| ----------- | -------- |
| Baud rate   | `115200` |
| Line ending | Any      |
| Output type | Text     |

Expected startup output:

```text
[GSM] Checking module
[GSM] Module ready
[SYSTEM] Started
```

If GSM is unavailable or not connected, the output may show:

```text
[GSM] Module not ready
[SYSTEM] Started
```

The system can still be tested without GSM if simulation mode is enabled in the firmware.

## Main Thresholds

The current firmware uses the following threshold values:

| Parameter                 |       Value | Description                                          |
| ------------------------- | ----------: | ---------------------------------------------------- |
| `TEMP_ALARM`              |        `30` | Temperature alarm threshold in °C                    |
| `TEMP_RESET`              |        `27` | Temperature reset threshold in °C                    |
| `GAS_ALARM`               |       `500` | Gas/smoke alarm threshold                            |
| `GAS_RESET`               |       `430` | Gas/smoke reset threshold                            |
| `MOTION_HOLD_TIME`        |   `2500 ms` | Time before closing the door after motion stops      |
| `ALARM_STABLE_RESET_TIME` |  `10000 ms` | Required safe period before returning to Normal mode |
| `SMS_REPEAT_TIME`         | `300000 ms` | Delay between repeated SMS alerts                    |
| `SENSOR_INTERVAL`         |    `250 ms` | Sensor reading interval                              |
| `STATUS_INTERVAL`         |   `1000 ms` | Serial status print interval                         |

The difference between alarm and reset thresholds is intentional. It creates hysteresis and prevents unstable switching between Normal and Alarm modes when sensor values fluctuate near the threshold.

## Test Case Summary

| ID    | Test Case              | Expected Result                                         | Status                 |
| ----- | ---------------------- | ------------------------------------------------------- | ---------------------- |
| TC-01 | System startup         | System initializes sensors, LEDs, servo and GSM logic   | Passed                 |
| TC-02 | Normal mode            | EXIT indication is active and sensor values are printed | Passed                 |
| TC-03 | Motion detection       | Door opens and evacuation path LEDs turn on             | Passed                 |
| TC-04 | Motion timeout         | Door closes after motion is no longer detected          | Passed                 |
| TC-05 | High temperature alarm | Alarm mode starts when temperature reaches threshold    | Passed                 |
| TC-06 | Gas/smoke alarm        | Alarm mode starts when gas value reaches threshold      | Passed                 |
| TC-07 | Combined alarm         | Alarm mode identifies both gas and high temperature     | Passed                 |
| TC-08 | Alarm reset            | System returns to Normal after values become safe       | Passed                 |
| TC-09 | GSM initialization     | GSM module responds to AT commands or reports not ready | Pending real hardware  |
| TC-10 | SMS sending            | SMS alert is sent during alarm mode                     | Pending real hardware  |
| TC-11 | SMS anti-spam          | SMS is not repeatedly sent too often                    | Passed by logic review |
| TC-12 | Fault mode             | System enters Fault mode for invalid sensor values      | Passed by logic review |
| TC-13 | Adaptive brightness    | LED brightness changes according to ambient light       | Passed                 |
| TC-14 | Buzzer alarm           | Buzzer turns on/off during Alarm mode                   | Pending real hardware  |

## Detailed Test Cases

## TC-01: System Startup

### Purpose

Verify that the system initializes correctly after power-up or reset.

### Preconditions

* Arduino is powered.
* Firmware is uploaded.
* Serial Monitor is opened at `115200` baud.
* Sensors and output components are connected according to the circuit diagram.

### Steps

1. Power on the Arduino.
2. Open Serial Monitor.
3. Wait for initialization messages.
4. Observe servo position.
5. Observe LED state.

### Expected Result

* Serial Monitor displays startup information.
* Servo moves to the closed door position.
* NeoPixel LEDs are cleared during initialization.
* GSM initialization is attempted.
* System enters `Normal` mode.

### Expected Serial Output

```text
[GSM] Checking module
[GSM] Module ready
[SYSTEM] Started
State: NORMAL | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: ...
```

If GSM is not connected:

```text
[GSM] Module not ready
[SYSTEM] Started
```

### Result

Passed if the system starts without freezing and begins printing status messages.

## TC-02: Normal Mode Operation

### Purpose

Verify that the system stays in Normal mode when no danger is detected.

### Preconditions

* Temperature is below `TEMP_ALARM`.
* Gas value is below `GAS_ALARM`.
* Motion sensor is inactive.

### Steps

1. Start the system.
2. Keep all sensor values in the safe range.
3. Observe Serial Monitor.
4. Observe EXIT indication.
5. Observe luster LEDs.

### Expected Result

* System remains in `Normal` state.
* Serial Monitor prints sensor values every second.
* EXIT indication is active.
* Luster LEDs are active with adaptive brightness.
* Door remains closed.
* No SMS alert is sent.
* Buzzer remains off.

### Expected Serial Output

```text
State: NORMAL | Reason: NONE | Temp: 24 | Gas: 250 | Light: 120 | IR: 0 | GSM: READY
```

### Result

Passed if the system remains stable in Normal mode and does not trigger alarm behavior.

## TC-03: Motion Detection

### Purpose

Verify that the system reacts to movement near the emergency exit.

### Preconditions

* System is in `Normal` mode.
* No gas or temperature alarm is active.
* IR sensor is connected.

### Steps

1. Trigger the IR motion sensor.
2. Observe the servo motor.
3. Observe the evacuation path LEDs.
4. Check Serial Monitor.

### Expected Result

* System switches from `Normal` to `Motion`.
* Door opens.
* Way LEDs turn on.
* Serial Monitor displays `MOTION`.
* No SMS is sent because motion alone is not an emergency alarm.

### Expected Serial Output

```text
[STATE] MOTION
State: MOTION | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: 1 | GSM: ...
```

### Result

Passed if motion causes the door to open and the system enters Motion mode.

## TC-04: Motion Timeout and Door Closing

### Purpose

Verify that the door closes automatically after motion stops.

### Preconditions

* System is in `Motion` mode.
* Motion sensor becomes inactive.

### Steps

1. Trigger motion.
2. Wait until the system enters `Motion` mode.
3. Stop triggering the motion sensor.
4. Wait for more than `MOTION_HOLD_TIME`.
5. Observe the servo and Serial Monitor.

### Expected Result

* Door remains open for a short hold time.
* After `MOTION_HOLD_TIME`, the door closes.
* Way LEDs turn off.
* System returns to `Normal` mode.

### Expected Serial Output

```text
[STATE] NORMAL
State: NORMAL | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: 0 | GSM: ...
```

### Result

Passed if the system closes the door after motion is no longer detected.

## TC-05: High Temperature Alarm

### Purpose

Verify that the system detects high temperature and enters Alarm mode.

### Preconditions

* Gas value is below `GAS_ALARM`.
* Motion state is not important.
* Temperature sensor is working.

### Steps

1. Increase the temperature value above `TEMP_ALARM`.
2. Observe Serial Monitor.
3. Observe servo motor.
4. Observe NeoPixel warning indication.
5. Observe buzzer behavior if connected.
6. Observe GSM/SMS behavior.

### Expected Result

* System switches to `Alarm` mode.
* Alarm reason is `TEMPERATURE`.
* Door opens.
* FIRE animation starts.
* Luster LEDs switch to maximum brightness.
* Buzzer starts if enabled.
* SMS alert is sent or simulated.

### Expected Serial Output

```text
[STATE] ALARM | Reason: TEMPERATURE
[GSM] Sending SMS
[GSM] SMS sent
State: ALARM | Reason: TEMPERATURE | Temp: 31 | Gas: ... | Light: ... | IR: ... | GSM: READY
```

If GSM is not available:

```text
[GSM] SMS was not sent
```

Or in simulation mode:

```text
[SMS SIMULATION] Увага! Температура перевищила безпечний рівень.
```

### Result

Passed if high temperature triggers Alarm mode and activates emergency behavior.

## TC-06: Gas or Smoke Alarm

### Purpose

Verify that the system detects gas or smoke and enters Alarm mode.

### Preconditions

* Temperature is below `TEMP_ALARM`.
* Gas sensor is connected and provides analog values.

### Steps

1. Increase gas sensor value above `GAS_ALARM`.
2. Observe Serial Monitor.
3. Observe way LEDs and FIRE animation.
4. Observe servo motor.
5. Observe GSM/SMS behavior.

### Expected Result

* System switches to `Alarm` mode.
* Alarm reason is `GAS`.
* Door opens.
* FIRE animation starts.
* SMS alert is sent or simulated.
* Buzzer starts if enabled.

### Expected Serial Output

```text
[STATE] ALARM | Reason: GAS
State: ALARM | Reason: GAS | Temp: ... | Gas: 520 | Light: ... | IR: ... | GSM: ...
```

Expected SMS text:

```text
Увага! Виявлено дим або газ у приміщенні.
```

### Result

Passed if gas/smoke detection triggers Alarm mode.

## TC-07: Combined Gas and Temperature Alarm

### Purpose

Verify that the system correctly detects a combined dangerous condition.

### Preconditions

* Temperature sensor is working.
* Gas sensor is working.

### Steps

1. Increase temperature above `TEMP_ALARM`.
2. Increase gas value above `GAS_ALARM`.
3. Observe Serial Monitor.
4. Observe emergency outputs.

### Expected Result

* System enters `Alarm` mode.
* Alarm reason is `GAS_AND_TEMPERATURE`.
* Door opens.
* Warning LEDs and FIRE animation activate.
* SMS alert mentions both gas/smoke and high temperature.

### Expected Serial Output

```text
[STATE] ALARM | Reason: GAS_AND_TEMPERATURE
State: ALARM | Reason: GAS_AND_TEMPERATURE | Temp: 31 | Gas: 520 | Light: ... | IR: ... | GSM: ...
```

Expected SMS text:

```text
Увага! Виявлено дим або газ і високу температуру.
```

### Result

Passed if the system identifies both danger factors at the same time.

## TC-08: Alarm Reset After Safe Conditions

### Purpose

Verify that the system returns to Normal mode only after the environment becomes safe and remains stable.

### Preconditions

* System is in `Alarm` mode.

### Steps

1. Trigger Alarm mode using gas or temperature.
2. Reduce temperature below `TEMP_RESET`.
3. Reduce gas value below `GAS_RESET`.
4. Keep values safe for at least `ALARM_STABLE_RESET_TIME`.
5. Observe system state.

### Expected Result

* Alarm does not stop immediately.
* System waits until safe values remain stable.
* After the stable reset time, system returns to `Normal`.
* Door closes.
* Buzzer stops.
* Way LEDs are cleared.
* EXIT indication returns.

### Expected Serial Output

```text
[STATE] NORMAL
State: NORMAL | Reason: NONE | Temp: 26 | Gas: 400 | Light: ... | IR: ... | GSM: ...
```

### Result

Passed if the system returns to Normal mode only after stable safe conditions.

## TC-09: GSM Module Initialization

### Purpose

Verify that the GSM module responds to basic AT commands.

### Preconditions

* SIM800L or compatible GSM module is connected.
* SIM card is inserted.
* GSM antenna is connected.
* External power supply is stable.
* `USE_GSM` is set to `true`.

### Steps

1. Power on the system.
2. Open Serial Monitor.
3. Observe GSM initialization messages.
4. Confirm that module checks are completed.

### Expected Result

The firmware checks the module using AT commands such as:

```text
AT
ATE0
AT+CMGF=1
AT+CPIN?
AT+CSQ
AT+CREG?
```

Expected Serial Output:

```text
[GSM] Checking module
[GSM] Module ready
```

If the module is not ready:

```text
[GSM] Module not ready
```

### Result

Passed if the GSM module responds and the system reports it as ready.

## TC-10: SMS Alert Sending

### Purpose

Verify that the system sends an SMS alert when an alarm condition is detected.

### Preconditions

* GSM module is connected and ready.
* SIM card has balance or SMS capability.
* Correct phone number is set in the firmware.
* `USE_GSM` is set to `true`.

### Steps

1. Trigger gas or temperature alarm.
2. Wait for GSM sending process.
3. Check Serial Monitor.
4. Check the recipient phone.

### Expected Result

* System sends an SMS alert.
* Recipient receives the correct warning message.
* Serial Monitor confirms sending.

### Expected Serial Output

```text
[GSM] Sending SMS
[GSM] SMS sent
```

Possible failure output:

```text
[GSM] SMS sending failed
```

### Result

Passed if the phone receives the SMS alert.

## TC-11: SMS Anti-Spam Logic

### Purpose

Verify that the system does not send SMS messages too frequently during a long alarm.

### Preconditions

* System is in `Alarm` mode.
* GSM module is ready.
* SMS has already been sent once.

### Steps

1. Trigger Alarm mode.
2. Confirm that the first SMS is sent.
3. Keep the alarm active.
4. Observe Serial Monitor for less than `SMS_REPEAT_TIME`.
5. Continue observing after `SMS_REPEAT_TIME`.

### Expected Result

* The first SMS is sent immediately after entering Alarm mode.
* Repeated SMS messages are blocked until `SMS_REPEAT_TIME` passes.
* After the repeat interval, another SMS may be sent if danger continues.

### Result

Passed if repeated SMS alerts are limited by the configured delay.

## TC-12: Fault Mode

### Purpose

Verify that the system can detect invalid sensor values and enter Fault mode.

### Preconditions

* Fault detection logic is enabled.
* Sensor values can be simulated or forced for testing.

### Steps

1. Force the temperature value outside the valid range.
2. Observe Serial Monitor.
3. Observe luster LEDs.
4. Restore valid sensor values.

### Expected Result

* System enters `Fault` mode.
* Door closes.
* Alarm buzzer stops.
* Luster LEDs blink as a fault indicator.
* When values return to normal, the system returns to `Normal` mode.

### Expected Serial Output

```text
[STATE] FAULT
State: FAULT | Reason: NONE | Temp: ... | Gas: ... | Light: ... | IR: ... | GSM: ...
```

After recovery:

```text
[STATE] NORMAL
```

### Result

Passed if invalid sensor values trigger Fault mode and recovery works correctly.

## TC-13: Adaptive Brightness

### Purpose

Verify that LED brightness changes depending on ambient light.

### Preconditions

* Photoresistor is connected.
* NeoPixel LEDs are working.

### Steps

1. Start the system in Normal mode.
2. Change the light level on the photoresistor.
3. Observe EXIT indication and luster LEDs.
4. Check that LEDs remain visible in low light and do not become too bright in high light.

### Expected Result

* LED brightness changes according to the light sensor value.
* Brightness does not fall below `MIN_BRIGHTNESS`.
* Luster brightness uses special day/night limits.

### Result

Passed if LED brightness adapts to ambient lighting.

## TC-14: Gas Level LED Indicator

### Purpose

Verify that the system displays gas level on the way LEDs during Normal mode.

### Preconditions

* System is in Normal mode.
* Gas sensor is connected.

### Steps

1. Keep gas value low.
2. Observe way LEDs.
3. Increase gas value gradually.
4. Observe the number of active LEDs.

### Expected Result

* Low gas values produce no or few active way LEDs.
* Higher gas values increase the number of active LEDs.
* If gas reaches `GAS_ALARM`, system enters Alarm mode.

### Result

Passed if gas level is visually represented before alarm threshold is reached.

## TC-15: Servo Door Control

### Purpose

Verify that the servo motor correctly represents the emergency door mechanism.

### Preconditions

* Servo is connected to the configured door pin.
* External power is sufficient if using real hardware.

### Steps

1. Start system.
2. Confirm door is closed.
3. Trigger motion.
4. Confirm door opens.
5. Stop motion and wait for timeout.
6. Trigger alarm.
7. Confirm door opens and stays open during alarm.

### Expected Result

| Condition   | Expected Servo Position |
| ----------- | ----------------------- |
| Startup     | Closed                  |
| Normal mode | Closed                  |
| Motion mode | Open                    |
| Alarm mode  | Open                    |
| Fault mode  | Closed                  |

### Result

Passed if servo behavior matches the system state.

## TC-16: FIRE Animation

### Purpose

Verify that the FIRE text animation appears during Alarm mode.

### Preconditions

* NeoPixel text matrix lines are connected.
* System is able to enter Alarm mode.

### Steps

1. Trigger gas or temperature alarm.
2. Observe text LEDs.
3. Keep alarm active for several seconds.

### Expected Result

* FIRE animation runs continuously.
* Animation updates according to `FIRE_FRAME_INTERVAL`.
* Way LEDs show a red moving emergency path indicator.

### Result

Passed if the animation is visible and continues during Alarm mode.

## TC-17: EXIT Animation

### Purpose

Verify that EXIT indication is shown during Normal mode.

### Preconditions

* System is in Normal mode.
* Text LED matrix lines are connected.

### Steps

1. Start the system.
2. Keep all sensor values safe.
3. Observe text LEDs.

### Expected Result

* EXIT indication is displayed.
* Brightness adapts to the photoresistor.
* Alarm text is not shown.

### Result

Passed if EXIT indication works during Normal mode.

## TC-18: Buzzer Alarm

### Purpose

Verify that the buzzer provides an additional alarm signal.

### Preconditions

* Buzzer is connected.
* `USE_BUZZER` is set to `true`.

### Steps

1. Trigger Alarm mode.
2. Listen for buzzer signal.
3. Return the system to safe conditions.
4. Confirm buzzer stops.

### Expected Result

* Buzzer toggles during Alarm mode.
* Buzzer stops after returning to Normal mode.
* Buzzer remains off during Normal and Motion modes.

### Result

Passed if buzzer behavior matches the alarm state.

## Test Results Table

| Test Case | Description            | Result  | Notes                                      |
| --------- | ---------------------- | ------- | ------------------------------------------ |
| TC-01     | System startup         | Passed  | System initializes and prints status       |
| TC-02     | Normal mode            | Passed  | EXIT indication and monitoring active      |
| TC-03     | Motion detection       | Passed  | Door opens and path LEDs activate          |
| TC-04     | Motion timeout         | Passed  | Door closes after timeout                  |
| TC-05     | High temperature alarm | Passed  | Alarm mode activates                       |
| TC-06     | Gas/smoke alarm        | Passed  | Alarm mode activates                       |
| TC-07     | Combined alarm         | Passed  | Gas and temperature detected together      |
| TC-08     | Alarm reset            | Passed  | System returns to Normal after safe period |
| TC-09     | GSM initialization     | Pending | Requires real SIM800L hardware             |
| TC-10     | SMS sending            | Pending | Requires real SIM800L hardware             |
| TC-11     | SMS anti-spam          | Passed  | Verified by firmware logic                 |
| TC-12     | Fault mode             | Passed  | Verified by firmware logic                 |
| TC-13     | Adaptive brightness    | Passed  | LED brightness follows light sensor        |
| TC-14     | Gas level indicator    | Passed  | Way LEDs show gas level                    |
| TC-15     | Servo door control     | Passed  | Servo reacts to state changes              |
| TC-16     | FIRE animation         | Passed  | Alarm animation works                      |
| TC-17     | EXIT animation         | Passed  | Normal indication works                    |
| TC-18     | Buzzer alarm           | Pending | Requires physical buzzer test              |

## Known Limitations

### GSM Testing Limitation

The GSM/SMS logic cannot be fully tested in Tinkercad because SIM800L and real mobile network behavior are not supported in the simulation environment.

For this reason:

* GSM initialization can be reviewed in code.
* SMS behavior can be simulated through Serial Monitor.
* Final SMS sending must be tested on real hardware.

### Sensor Accuracy Limitation

Simulated sensors do not always behave like real physical sensors. Real MQ-2 and TMP36 readings may vary depending on calibration, wiring, power supply, environment and sensor warm-up time.

### Power Supply Limitation

Real NeoPixel LEDs, servo motors and SIM800L modules may require more current than Arduino UNO can provide directly. For real hardware testing, an external power supply is recommended.

## Hardware Testing Recommendations

For real hardware testing, check the following:

* Use a stable external power supply for SIM800L.
* Connect common GND between Arduino, GSM module, LEDs and servo.
* Use a SIM card with SMS support.
* Attach GSM antenna before testing.
* Avoid powering SIM800L directly from Arduino 5V pin.
* Test GSM commands separately before full system testing.
* Test sensors one by one before running the complete system.
* Verify servo movement without mechanical load first.
* Check NeoPixel current consumption before setting high brightness.

## Final Testing Conclusion

The system successfully implements the main behavior required for an Arduino-based emergency warning system:

* It reads environmental sensor values.
* It detects motion, gas/smoke and high temperature.
* It reacts automatically to dangerous conditions.
* It opens the emergency door using a servo motor.
* It activates visual warning indication using NeoPixel LEDs.
* It displays system diagnostics in the Serial Monitor.
* It includes GSM/SMS notification logic for real hardware usage.
* It supports safer operation through hysteresis, sensor filtering, alarm reset delay and SMS anti-spam logic.

The project is suitable as a portfolio-level embedded systems prototype. Full validation of GSM/SMS functionality requires real SIM800L hardware testing.
