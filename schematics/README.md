# Schematics

This folder contains wiring diagrams and prototype layout images for the Arduino-based emergency warning system.

## Available Files

| File | Description |
|---|---|
| `tinkercad-prototype-layout.png` | Original Tinkercad prototype wiring layout used during the thesis development and simulation stage |

## Prototype Layout

The file `tinkercad-prototype-layout.png` shows the main Arduino prototype layout.

It includes:

- Arduino UNO;
- motion sensor;
- servo motor;
- gas/smoke sensor;
- temperature sensor;
- photoresistor;
- NeoPixel LED indicators;
- breadboard wiring;
- external power connection for LED components.

## Important Note

This diagram represents the original prototype layout used during simulation and thesis development.

The current repository firmware has been improved and includes additional functionality such as:

- GSM/SMS logic;
- buzzer support;
- improved state-based firmware;
- fault mode;
- SMS anti-spam logic;
- GSM readiness checking.

Some of these improvements may not be fully shown in the original Tinkercad diagram.

For the current firmware pin configuration, see:

[`../docs/hardware.md`](../docs/hardware.md)

For the firmware architecture, see:

[`../docs/firmware-architecture.md`](../docs/firmware-architecture.md)

## Usage

This image can be used as a visual reference for understanding the prototype structure.

For real hardware assembly, always verify the wiring with the current firmware constants in `src/main.cpp` and the pin table in `docs/hardware.md`.

## Limitations

The diagram should be treated as a prototype layout, not as a certified final electrical schematic.

For real hardware use, the system would require:

- proper power design;
- stable external supply for SIM800L;
- stable external supply for NeoPixel LEDs and servo if needed;
- common GND between all modules;
- protection components;
- real sensor calibration;
- real GSM testing.
