# Codex Continuation Prompt

Paste the following into a new Codex chat opened in this project folder:

---

Continue mentoring me through the STM32 environmental-monitor project in this
repository. First read `AGENTS.md`, `PROJECT_CONTEXT.md`,
`PROJECT_IDEAS_AND_ROADMAP.md`, `HARDWARE_INVENTORY.md`, and especially the latest
dated section of `WHERE_WE_LEFT_OFF.md`. Inspect the current repository before
proposing changes; the repository is authoritative if this prompt differs from it.

The primary board is a NUCLEO-F446RE with an STM32F446RET6. Firmware is the
CubeMX-generated STM32CubeF4 HAL C project under `firmware/stm32_telemetry`, built
with CMake, Ninja, and GNU Arm GCC. USART2 uses PA2/PA3 through the onboard ST-LINK
Virtual COM Port on COM3 at 115200 8-N-1. LD2 is PA5 and B1 is active-low on PC13.

Stages 0 through 5 are complete:

- LD2 toggles every 250 ms while B1 is released and stays on while B1 is held;
- UART reports startup, debounced B1 events, and sensor telemetry every two seconds;
- ADC1 scan mode polls rank 1 PA0/TMP36 and rank 2 PA1/photoresistor;
- integer math reports TMP36 raw count, nominal-reference millivolts, signed
  temperature in tenths of a degree Celsius, and raw light count;
- warming the TMP36 increased its reported temperature as expected;
- covering the photoresistor reduced `light_raw` from about 3720 to about 1768,
  and uncovering restored it;
- the Debug build succeeds without warnings at 14,704 bytes flash and 2,136 bytes
  RAM;
- firmware and CI implementation through `1f3ab23` is pushed to `origin/main`.

The physical wiring most recently verified is:

- TMP36 pin 1/+VS to NUCLEO 3V3;
- TMP36 pin 2/VOUT to A0/PA0;
- TMP36 pin 3/GND to NUCLEO GND;
- 3V3 through the photoresistor to a measurement node;
- that measurement node to A1/PA1 and one side of a 10 kOhm resistor;
- the other side of the 10 kOhm resistor to GND.

GitHub Actions workflow `.github/workflows/firmware-ci.yml` runs on pushes to
`main`, pull requests, and manual dispatch. It builds Release firmware and directly
uploads a UTC-timestamped ZIP containing ELF, BIN, Intel HEX, linker map, memory
usage, build provenance, SHA-256 checksums, and `flash-firmware.ps1`. The first run,
`34832037856`, succeeded. The user downloaded
`stm32-telemetry-20260914-101326-utc.zip`, extracted it, and successfully used the
packaged script to verify and flash the real NUCLEO over ST-LINK/SWD.

The required long-term learning path includes deeper bidirectional UART work,
message framing/parsing and error handling, I2C with a real sensor driver, SPI with
a real sensor or peripheral driver, and additional sensors. Before using Hetzner,
build a local end-to-end path: a Python gateway reads COM3 and publishes sensor
telemetry to a Mosquitto MQTT broker running in Docker on the laptop. The gateway
should initially supply the wall-clock timestamp.

The eventual dream project is a compact custom PCB with a microcontroller,
temperature sensor, power/debug circuitry, a reliable timestamp strategy, and an
optional Bluetooth or LTE module that sends temperature data to a remote service.
RF-capable imports into Israel can involve difficult customs checks and fees, so
keep transport replaceable and do not let RF sourcing block progress. A first PCB
may expose UART/SPI and power to a separate locally obtainable radio module. Verify
current Israeli requirements and viable local/pre-approved parts before selecting
or directly integrating RF hardware.

Begin Stage 6: separate pure ADC/sensor conversion logic from HAL and peripheral
code. Briefly explain why pure functions can compile and run as host tests without
STM32 hardware. Propose a small application-owned C module for ADC-count-to-mV and
TMP36 temperature conversion, then give me one small coding task and review my work
before implementing the rest. Add host-side unit tests for boundary and
representative cases, including raw counts 0 and 4095 and a below-zero TMP36 case.
Extend GitHub Actions so tests must pass before firmware packaging.

After Stage 6, address explicit ADC start/stop/poll error handling and clean
structured telemetry. Then cover bidirectional UART and build the laptop
serial-to-MQTT gateway. Follow with I2C, SPI, and additional sensors before moving
the telemetry stack to Hetzner. Do not jump directly to DMA, RTOS, hardware-in-the-
loop infrastructure, RF integration, or Hetzner deployment.

This is a hands-on embedded C learning project. Work in small stages, explain the
basic mental model first, let me write important embedded C code, and review errors
rather than silently fixing them unless I explicitly ask. Keep application edits
inside CubeMX `USER CODE` regions and put new application-owned modules outside
generated code where appropriate.

Preserve unrelated working-tree content: `AGENTS.md`, `tmp/`,
`firmware/stm32_telemetry/.settings/`, and
`firmware/stm32_telemetry/.gitignore`. Do not stage, modify, delete, or commit them
unless I explicitly request it.

---
