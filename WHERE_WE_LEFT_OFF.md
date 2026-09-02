# Where We Last Left Off

## Date

August 31, 2026

## Current Situation

The user decided that the embedded + DevOps project should be moved into Codex because Codex is better suited for direct repository and implementation work.

The user does not want a six-month learning curriculum.

The user wants the assistant/Codex to act as:
- lead Embedded Software Developer
- DevOps engineer
- DevSecOps engineer

The user will primarily perform physical actions that require hands-on access to the hardware.

## Known Hardware

The user said they already have:
- Arduino
- STMicroelectronics NUCLEO-F446RE
- Arduino starter pack with many sensors
- jumper cables
- 2 breadboards

The exact sensor/module inventory is not known yet.

## Server

The user already has a Hetzner server and wants to reuse it for this project.

Server specifics have NOT yet been inventoried in this handoff.

Do not assume ports, Docker state, running applications, firewall configuration, or DNS.

## Agreed Direction

The preferred project is one evolving end-to-end system:

Sensors
   |
   v
NUCLEO-F446RE
   |
   v
Embedded C firmware
   |
   v
Telemetry transport
   |
   v
Hetzner
   |
   +--> MQTT
   +--> backend/API
   +--> PostgreSQL
   +--> dashboard / observability

Then add:
- automated firmware builds
- unit tests
- static analysis
- GitHub Actions
- versioned firmware artifacts
- hardware-in-the-loop testing
- RTOS / Zephyr
- signed firmware releases

## Exact Point Where Work Stopped

Before beginning implementation, the next step was:

**Identify the user's exact hardware inventory.**

The assistant had asked the user to show:
- NUCLEO board
- Arduino board
- sensors/modules
- jumper wires
- resistors/components
- USB cables as relevant

The user then realized the conversation was in regular ChatGPT rather than Codex and requested this handoff.

Therefore:

**Do not start selecting sensors or giving pin-by-pin wiring until the hardware inventory is known.**

## Codex Progress Update - September 1, 2026

The original inventory blocker has been substantially resolved. Photos confirmed the
NUCLEO-F446RE (MB1136 rev C), Arduino Uno Rev3, SparkFun Essential Sensor Kit V2,
Seeed Sidekick Basic Kit V2, cables, and loose SparkFun sensor parts.

Milestone 1 hardware is selected:

- TMP36 analog temperature sensor;
- photoresistor with a 10 kOhm voltage-divider resistor;
- NUCLEO onboard B1 user button and LD2 user LED;
- USB/UART telemetry through the onboard ST-LINK Virtual COM Port.

The NUCLEO was connected and verified successfully with STM32CubeProgrammer 2.20.0:

- ST-LINK and SWD are operational;
- target voltage is 3.24 V;
- STM32F446xx target, revision A, 512 KBytes flash detected;
- ST-LINK Virtual COM Port enumerated as `COM3`;
- no firmware has yet been erased or flashed.

The VS Code-based STM32 development toolchain was installed and verified:

- Visual Studio Code 1.135.0;
- STM32CubeIDE for Visual Studio Code extension pack 3.10.0;
- GNU Tools for STM32 / `arm-none-eabi-gcc` 14.3.1;
- CMake 4.3.1;
- Ninja 1.13.2;
- STM32CubeProgrammer and ST-LINK debugging tools.

STM32CubeMX 6.18.1 and STM32CubeF4 1.28.3 are now installed. CubeMX generated the
initial CMake/HAL project at `firmware/stm32_telemetry` with:

- SWD debugging on PA13/PA14;
- LD2 output on PA5 and B1 input on PC13;
- USART2 on PA2/PA3 at 115200 baud;
- ADC1 channels on PA0 and PA1;
- the default 16 MHz HSI system clock.

The pristine generated project compiled successfully with GNU Arm GCC 14.3.1.
A board-only smoke test was then added in CubeMX-protected user-code sections. It
blinks LD2 every 500 ms and reports startup, B1 press, and B1 release events through
the ST-LINK Virtual COM Port. That build also passed, using 10,848 bytes (2.07%) of
flash and 1,728 bytes (1.32%) of RAM.

The user built and flashed the smoke-test firmware successfully. The following were
physically verified on the NUCLEO:

- LD2 blinks every 500 ms;
- COM3 works at 115200 8-N-1;
- the UART startup message is received after reset;
- pressing and releasing B1 produces the expected UART messages.

Work paused before sensor wiring because the user needs to return to the physical
hardware later. The NUCLEO USB cable was disconnected, so the board was left
unpowered.

### Exact next physical step: TMP36 on ADC1 channel 0

Before applying power, confirm that the flat face is marked `TMP36`, `TMP36GZ`, or
`TMP36GT9Z`. With the flat marked face toward the user and the three legs pointing
downward, the pins from left to right are `+VS`, `VOUT`, and `GND`.

Place the TMP36 on the breadboard so that every leg is in a different electrically
connected five-hole strip. Then connect:

- TMP36 left leg (`+VS`) to the NUCLEO pin labeled `3V3` using a red jumper;
- TMP36 center leg (`VOUT`) to the NUCLEO Arduino pin labeled `A0` using a yellow jumper;
- TMP36 right leg (`GND`) to a NUCLEO pin labeled `GND` using a black jumper.

Use `3V3`, not `5V`. Keep USB disconnected while wiring. Before restoring power,
take a clear overhead photo showing the TMP36 orientation, its breadboard rows, and
all three NUCLEO endpoints so the wiring can be checked.

## Immediate Next Action in Codex

1. Read:
   - `AGENTS.md`
   - `PROJECT_CONTEXT.md`
   - `PROJECT_IDEAS_AND_ROADMAP.md`
   - `HARDWARE_INVENTORY.md`
   - this file

2. Confirm the TMP36 package marking and wire it to `3V3`, `A0/PA0`, and `GND` as
   documented above while the NUCLEO remains unpowered.

3. Review an overhead wiring photo before reconnecting USB power.

4. Add ADC sampling and calibrated TMP36 temperature telemetry to the firmware.

5. Wire and validate the photoresistor divider on `A1/PA1`.

6. Emit structured sensor telemetry over UART/USB serial.

7. Add CI immediately after basic sensor bring-up.

## Do Not

- send the user off to read a long STM32 course first
- make them research pin mappings themselves
- create several toy projects
- overengineer the initial milestone
- use Arduino abstractions as the primary embedded architecture
- deploy to Hetzner before checking what is already running there
