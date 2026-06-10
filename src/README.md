# Source Code

This folder contains the main Arduino firmware for the emergency warning system.

## Files

| File | Description |
|---|---|
| `main.cpp` | Main Arduino C++ firmware file |

## Firmware Overview

The firmware implements the main logic of the warning system:

- sensor reading;
- gas/smoke detection;
- temperature monitoring;
- motion detection;
- NeoPixel indication;
- servo door control;
- buzzer alarm;
- GSM/SMS notification logic;
- Serial Monitor diagnostics.

## Main Architecture

The firmware is based on a state-machine approach with the following modes:

- `Normal`
- `Motion`
- `Alarm`
- `Fault`

For a detailed explanation of the firmware structure, see:

[`../docs/firmware-architecture.md`](../docs/firmware-architecture.md)
