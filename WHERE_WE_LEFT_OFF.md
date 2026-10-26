# Where We Last Left Off

## Latest STM32 continuation point - October 26, 2026

This supersedes older STM32 next-action and validation notes. Nano/BLE work below
remains separate. Prioritize embedded learning and hardware validation; defer
cloud infrastructure and custom PCB work.

### UART reception verified on hardware

The user restored the circuit and observed schema-1 JSON telemetry on COM3 at
115200 8-N-1 and LD2 activity. In VS Code with the ST STM32 extension, they selected
Debug and launched ST-LINK GDB. Their log confirms programming the Debug ELF and
stopping at main.c:87 before HAL_Init(). They used WATCH, pause, and continue.

Decisive test: breakpoint at main.c:289 BEFORE uart_rx_count++. Sending C from
COM3 hit the callback with uart_rx_byte=67 ('C') and uart_rx_count=6. Continuing
produced no second receive stop; removing the breakpoint and pausing showed 7.
This verifies that character's reception, callback, increment, and continued
reception after earlier bytes (rearming). Earlier extra increments remain
unexplained; no terminal line-ending mode or extra-byte identity was established.
This does not prove sustained reception, error recovery, or command handling.

The user learned the vector table -> USART2_IRQHandler -> HAL_UART_IRQHandler ->
HAL receive completion -> weak-callback override in main.c path. A byte is eight
bits; 8-N-1 uses ten wire bits. Telemetry is ASCII JSON plus CRLF. WATCH being
unavailable while running and SIGINT on manual pause were normal debugger behavior.

No firmware C edits or automated tests were performed this session. The callback
still overwrites one byte, counts it, and rearms reception. CommandFrame is already
included and initialized before the loop, but UART bytes are not queued or passed
to it. STATUS parser dispatch/responses, UART-error recovery, partial-frame
timeouts, and dedicated framer discard-recovery tests remain outstanding.

### ADC discrepancy remains unresolved

New clip leads enabled DMM measurements: supply 3.31 V; TMP36 generally around
0.735-0.739 V while individual UART samples had wider excursions (one captured
set: 668-821 mV). Excursions exist in temp_raw, not just conversion. The nominal
3300 mV assumption does not explain them. DMM averaging may hide transients;
no scope evidence or root cause was established. Office AC can change baseline.

User-reported controlled checks:
- A0 disconnected from sensor and grounded: temp_mv about 0-1 mV. This checks a
  low-impedance zero input, not all ADC behavior with the sensor attached.
- Restoring sensor output to A0 brought excursions back.
- A 100 nF ceramic across TMP36 supply/GND did not resolve the spikes.
- Divider disconnected from A1, A1 grounded: light_raw 0-4 counts; temperature
  still 715-751 mV across ten samples. No conclusion that large spikes are gone.
- DMM between Nucleo GND and sensor breadboard GND displayed 0 V: no detectable
  difference, not proof of no transient ground noise.

Do not hide the issue with averaging or claim calibrated accuracy. ADC scan/poll
sequencing has not yet been fully investigated.

### Next session and physical state

Explain a bounded receive buffer shared by an interrupt producer and main-loop
consumer; ask the user to propose a design before implementation. Discuss ownership,
indices, full/empty conditions, overflow, and why volatile alone is not synchronization.
Inspect blocking UART sends, ADC polls, and the 20 ms debounce delay. Keep ISR work
short. Progress toward framer/parser integration in small reviewed tasks.

Keep the user as primary developer. Do not edit learning C without explicit
authorization. Leave builds, tests, and flashing to the user unless requested.
No new test-suite results or CI status were checked today.

The user was told to restore divider junction -> A1 after removing A1-GND, remove
the breakpoint, stop debugging (Shift+F5), close COM3, and unplug USB. Restoration
and shutdown were not confirmed; check actual wiring/power next session.

Preserve unrelated work. Git showed modified AGENTS.md and HARDWARE_INVENTORY.md;
untracked docs/, hardware/, research/, tmp/, firmware/stm32_telemetry/.clangd,
.gitignore, and .settings/. Never stage these automatically or use git add .
Handoff/prompt files were historically local Git-ignored notes. The user requested
syncing this session's documentation to GitHub, including these two files.

## Bench equipment update - October 26, 2026

The user has received banana-plug clip leads for the multimeter. An extra jumper
from a breadboard contact strip can now provide a convenient clip contact,
addressing the earlier probe-fit difficulty. No new measurements or hardware
validation have been reported. Voltage is measured across two nodes; current
requires opening the intended branch and inserting the meter in series with the
correct socket/range. Never connect current-mode leads across the power rails.
See HARDWARE_INVENTORY.md for the measurement setup notes. This equipment update
does not change the firmware continuation point below.

## Latest continuation point - October 15, 2026

This section supersedes older next-action and hardware-status sections below.
The historical STM32 learning work remains separate and unchanged by this session.

### EverCold hardware and verified milestone

The Nano 33 BLE Sense Rev2 with headers, Micro-USB cable, and SparkFun multimeter
have arrived. Arduino IDE uses Arduino Nano 33 BLE, currently COM4. The developer
installed Arduino Mbed OS Nano Boards and ArduinoBLE, successfully uploaded Blink,
and changed its timing. The old TMP36/light circuit was rebuilt using Nano 3V3
and shared GND: TMP36 output to A0; light-divider junction to A1, with
3V3 -> photoresistor -> junction -> 10 kOhm -> GND. Inputs are not 5 V tolerant.

The developer verified raw 12-bit ADC readings, plausible TMP36 Celsius conversion,
both sensors responding to physical changes, BLE discovery/connection, and live
temperature notifications in the laptop Python receiver. Typical displayed
temperatures were around 24-26 C. This establishes bench signal behavior, not
calibrated accuracy. Multimeter checks were deferred because the probes could
not make stable breadboard contact; mini grabbers or suitable leads are needed.

### Canonical files and operation

- Arduino sketch: firmware/nano_ble/EverColdAnalogTest/EverColdAnalogTest.ino.
- Laptop receiver: gateway/ble_receiver.py.
- Python dependency: gateway/requirements-ble.txt pins bleak==3.0.2.
- Setup/upload/run instructions: firmware/nano_ble/README.md.
- Short inline explanations were added to both programs without changing behavior.

Open the repository sketch for future Arduino IDE edits. The older sketchbook
copy remains at C:/Users/Owner/Documents/Arduino/EverColdAnalogTest/ and is no
longer the canonical source. The receiver was moved from firmware/ble_receiver.py.
The existing Python environment is firmware/.venv/; it remains usable, is not
relocated, and .venv/ directories are now ignored by Git. From the repo root:

```powershell
.\firmware\.venv\Scripts\python.exe .\gateway\ble_receiver.py
```

The sketch samples every 2000 ms using millis(), while BLE.poll() runs on each
loop pass. Keep Serial Monitor open at 115200: setup waits for USB serial before
starting BLE. USB powers the board; the Python receiver obtains temperature over
Bluetooth, not serial. It discovers the advertised name EverCold and subscribes
to the float characteristic. Light readings are currently USB-only.

Service UUID: 7e400001-b5a3-f393-e0a9-e50e24dcca9e.
Temperature UUID: 7e400002-b5a3-f393-e0a9-e50e24dcca9e.
Payload: four-byte little-endian float, degrees Celsius. Receiver exits on
disconnect; reconnect behavior is not implemented. Ctrl+C stops the receiver.

### Next step and preferences

Next: explain MQTT publisher/broker roles, then incrementally add publishing to
the working Python receiver. The user wants local laptop testing followed by
Hetzner. No MQTT broker, MQTT publishing, server deployment, or dashboard was
implemented in this session. Inspect Hetzner state before deploying anything.
The onboard HS3003 is available but this verified exercise uses the external TMP36.

Keep explanations short and the user as the primary developer. Leave firmware
builds, tests, and flashing to the user unless explicitly requested otherwise.
Existing GitHub Actions validate/build STM32 only; Nano compilation and BLE
hardware verification are not covered by CI. Preserve unrelated local changes.

This handoff is intentionally Git-ignored and stays local. The user authorized
committing and pushing the Nano/BLE files and related repository documentation
to the current Embedded repository (origin: maxhat180/STM32-Project-1), not the
separate EverCold repository. Commit b3aba64 (Add Nano BLE sensor prototype and
Python receiver) was successfully pushed to origin/main on October 15, 2026.
It includes seven scoped source/documentation files; unrelated local changes
were not staged. Diff whitespace checks passed. No builds or tests were rerun;
the hardware and BLE results above were reported by the developer. CI status
has not been checked for this push.

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

## Codex Progress Update - September 9, 2026

This is the latest and authoritative continuation point. It supersedes the older
"Immediate Next Action in Codex" section above.

The project was re-inspected as a learning-first environmental-monitor project.
Stage 0 repository and hardware orientation was completed without changing the
generated architecture. The user reviewed the roles of CubeMX, CMake, Ninja, GNU
Arm GCC, the linker script, ELF firmware, STM32CubeProgrammer, ST-LINK, and SWD.

The existing board-only smoke test was rebuilt, flashed, and physically verified:

- Windows detected the ST-LINK Virtual COM Port as `COM3`;
- UART worked at 115200 baud, 8 data bits, no parity, and 1 stop bit;
- reset produced the expected startup text;
- B1 press and release events produced the expected UART messages;
- LD2 and B1 both behaved correctly.

Stage 1 GPIO work was then completed by the user in
`firmware/stm32_telemetry/Core/Src/main.c`:

- the LED toggle interval is now the compile-time macro
  `LED_TOGGLE_INTERVAL_MS`, set to `250U` in a CubeMX-protected user section;
- while B1 is released, LD2 toggles every 250 ms;
- while active-low B1 is held, LD2 is forced steadily on;
- the existing debounced UART event reporting remains intact.

The user learned and physically confirmed that a toggle interval is half of a
complete on/off cycle, that the board's electrical circuit determines the
active-low input behavior, and that mutually exclusive `if`/`else if` control
prevents timed toggling while the button is held.

The user configured and built the Debug preset with GNU Arm GCC 14.3.1. After the
source-only change, Ninja performed two incremental steps: recompiling `main.c`
into its object file and relinking the ELF while reusing unchanged object files.
The build completed without warnings and reported:

- flash: 10,864 bytes of 512 KB (2.07%);
- RAM: 1,728 bytes of 128 KB (1.32%).

STM32CubeProgrammer CLI successfully wrote, verified, and reset the firmware. The
new LED behavior and the unchanged UART behavior were both physically verified.
The NUCLEO is currently connected and powered. No external sensors are wired.

### Exact next learning stage

Begin Stage 2: UART. First explain the existing data path from `UART_Write()` and
`HAL_UART_Transmit()` through USART2 PA2/PA3, the onboard ST-LINK Virtual COM Port,
and Windows COM3. Then give the user one small UART coding exercise, let the user
implement it, review the change, build, flash, and physically test it. Do not skip
ahead to ADC or sensor wiring until the Stage 2 learning check is complete.

### Working-tree caution

`AGENTS.md` and `tmp/` contain unrelated local changes and must not be included in
the Stage 0/1 progress commit unless the user explicitly requests it.

## Codex Progress Update - September 10, 2026

This is the latest and authoritative continuation point. It supersedes the older
next-action sections above.

Stage 2 UART transmit work is complete. The user added a non-blocking two-second
UART heartbeat using `HAL_GetTick()` and the existing `UART_Write()` wrapper. The
firmware was built, flashed, and physically verified without changing the existing
LD2 or B1 behavior. The UART path through USART2 PA2/PA3, the onboard ST-LINK
Virtual COM Port, and Windows COM3 was reviewed. VS Code go-to-definition remains
unresolved: the C/C++ and STM32Cube clangd extensions are installed, and the project
was configured as an STM32Cube CMake project, but symbol navigation still did not
work. This is not blocking firmware development.

Stage 3 ADC bring-up has started. The user identified and wired the TMP36GZ while
the board was unpowered, then completed a powered smoke test. Current wiring is:

- TMP36 pin 1 (`+VS`) to NUCLEO `3V3`;
- TMP36 pin 2 (`VOUT`) to NUCLEO Arduino `A0` / MCU `PA0`;
- TMP36 pin 3 (`GND`) to NUCLEO `GND`.

The NUCLEO remains connected and powered, and the TMP36 remains wired. The user
confirmed normal LD2/UART operation and no sensor heating.

CubeMX was intentionally simplified from a two-channel PA0/PA1 ADC scan to a
single-channel PA0 conversion for this polling-based learning stage. The generated
ADC1 configuration is now:

- PA0 / ADC1 channel 0 only; PA1 is unassigned;
- 12-bit, right-aligned conversion;
- scan and continuous conversion disabled;
- one software-triggered regular conversion;
- PCLK2 divided by 4;
- 84-cycle sampling time;
- EOC after the single conversion;
- DMA disabled.

Application code in `Core/Src/main.c` now samples ADC1 every two seconds using
`HAL_ADC_Start()`, `HAL_ADC_PollForConversion()`, `HAL_ADC_GetValue()`, and
`HAL_ADC_Stop()`. It formats the raw count into a bounded 32-byte buffer with
`snprintf()` and sends `ADC raw=<value>` through the existing UART wrapper. The
poll result and formatted length are validated; the start/stop return values are
temporarily ignored for this first learning pass and should receive explicit error
handling later.

The Debug build completed without warnings or errors using GNU Arm GCC 14.3.1:

- flash: 14,548 bytes of 512 KB (2.77%);
- RAM: 2,136 bytes of 128 KB (1.63%).

STM32CubeProgrammer 2.20.0 wrote and verified the ELF successfully, then reset the
MCU. Target voltage was 3.25 V. A direct ten-second COM3 capture at 115200 8-N-1
produced stable TMP36 readings from approximately 868 to 881 counts, centered near
875. Using 3.25 V as an initial reference estimate, this is about 694 mV and 19.4 C,
which is plausible room temperature. This is not yet a calibrated measurement.
COM3 was closed cleanly after capture and is currently free.

### Exact next learning step

Explain the 12-bit ADC conversion from raw counts to millivolts, then the TMP36
transfer function from millivolts to Celsius. Give the user one small integer-math
coding task that adds voltage and temperature to the existing two-second UART line.
Use a signed type for temperature so values below 0 C remain representable, avoid
floating-point `printf`, review the user's code before building, and then physically
verify that gently warming the sensor changes the reading in the expected direction.

Do not add PA1/photoresistor, DMA, CI/CD, or server work until this Stage 3
temperature-conversion check is complete.

### Working-tree caution after Stage 3 bring-up

`AGENTS.md` and `tmp/` remain unrelated local changes and must not be committed.
The STM32 VS Code configuration flow also created untracked
`firmware/stm32_telemetry/.settings/` and
`firmware/stm32_telemetry/.gitignore`; they were treated as incidental local IDE
metadata and intentionally excluded from this progress commit.

## Codex Progress Update - September 14, 2026

This is the latest and authoritative continuation point. It supersedes every
older next-action section above.

### Stage 3 complete: TMP36 temperature conversion

The user extended the two-second ADC telemetry with integer-only conversion from
the 12-bit ADC count to millivolts and TMP36 temperature:

```text
voltage_mV = raw * 3300 / 4095
temperature_x10_C = voltage_mV - 500
```

Temperature uses a signed type so sub-zero values remain representable, and the
firmware avoids floating-point `printf`. Typical ambient readings were about
21.8-22.4 C. Gently warming the TMP36 raised the reported value to about 24.7 C,
then it fell toward ambient after release. This physically verified the complete
sensor-to-UART path. Commit `287fd57` records this stage.

### Stage 4 complete: two-channel ADC and photoresistor

CubeMX now configures ADC1 as a two-rank polling sequence:

- rank 1: `PA0 / ADC1_IN0`, TMP36, 84-cycle sampling;
- rank 2: `PA1 / ADC1_IN1`, photoresistor divider, 84-cycle sampling;
- 12-bit right-aligned data;
- scan mode enabled with two conversions;
- software trigger, continuous/discontinuous conversion disabled;
- end-of-conversion after each conversion;
- DMA disabled.

The application starts ADC1 once, separately polls and reads rank 1 and rank 2,
then stops ADC1. It reports temperature raw count, nominal-reference millivolts,
temperature in tenths of a degree Celsius, and raw light count on one UART line.

The photoresistor divider is wired as:

```text
3V3 -- photoresistor --+-- A1 / PA1
                       |
                     10 kOhm
                       |
                      GND
```

With this orientation, brighter light produces a higher ADC count. The user
physically verified approximately 3720 uncovered and 1768 covered, with the value
returning to approximately 3729 when uncovered. The TMP36 continued reporting
stable ambient temperature. The Debug build completed without warnings at 14,704
bytes flash (2.80%) and 2,136 bytes RAM (1.63%). Commit `7517256` records this
stage.

### Stage 5 complete: firmware CI artifact and one-command flashing

The repository now contains `.github/workflows/firmware-ci.yml`. It runs on pushes
to `main`, pull requests, and manual dispatch. The hosted Ubuntu job:

1. installs the ARM bare-metal toolchain, Newlib, Ninja, and ZIP tooling;
2. configures and builds the CMake Release preset;
3. generates ELF, BIN, Intel HEX, linker-map, memory-usage, provenance, and
   SHA-256 checksum files;
4. packages them as a UTC-timestamped ZIP such as
   `stm32-telemetry-20260914-101326-utc.zip`;
5. uploads that ZIP directly as a 30-day GitHub Actions artifact.

Third-party workflow execution is minimized. The official GitHub checkout and
upload-artifact actions are pinned to their full v7.0.1 commit SHAs, workflow
permissions are read-only, and checkout credentials are not persisted.

The first hosted run, GitHub Actions run `34832037856`, passed every step in 35
seconds. Its artifact was downloaded and independently checksum-tested. The user
then downloaded the same artifact, extracted it under Windows Downloads, and ran
the packaged `flash-firmware.ps1`. The script verified the Intel HEX SHA-256,
flashed through STM32CubeProgrammer over ST-LINK/SWD, verified the write, and reset
the MCU successfully. This proves the full path from Git push through hosted build
artifact to physical-board deployment. Commit `1f3ab23` records this stage.

The repository copy of the helper can also find the newest matching artifact ZIP
in the Windows Downloads folder automatically:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File `
  "C:\Users\Owner\Documents\Work\Embedded\tools\flash-firmware.ps1"
```

### Current hardware state

Most recently verified:

- NUCLEO-F446RE connected over its ST-LINK USB connection;
- TMP36 wired to `3V3`, `A0/PA0`, and `GND`;
- photoresistor/10 kOhm divider wired to `3V3`, `A1/PA1`, and `GND`;
- LD2, B1, USART2/COM3, temperature telemetry, and light telemetry operational;
- the GitHub-built Release artifact flashed successfully and ran on the board.

Do not assume COM3 is still open when resuming; inspect it as needed.

### Long-term direction clarified

The project must remain one coherent system while deliberately covering:

- more UART, including receive, framing/parsing, commands, timeouts, and recovery;
- I2C with a real sensor driver;
- SPI with a real sensor or peripheral driver;
- additional sensors from the available inventory;
- a local end-to-end telemetry path in which a Python laptop gateway reads the
  NUCLEO USB serial stream and publishes to a Mosquitto MQTT broker running in
  Docker on the laptop;
- migration of that proven MQTT/ingestion path to Hetzner later.

The user's long-term dream project is a compact custom PCB containing a
microcontroller, temperature sensor, power/debug support, a timestamp strategy,
and eventually a Bluetooth or LTE module that sends timestamped temperature data
to a remote service. Bluetooth generally requires a nearby gateway, whereas LTE
can provide independent connectivity at greater power and integration complexity.

Importing RF-capable components into Israel may involve difficult customs checks
and fees, so RF sourcing is an explicit project constraint. Preserve a replaceable
transport boundary and do not allow RF procurement to block firmware, local MQTT,
or a first custom PCB. A sensible first PCB can expose UART/SPI and power for a
separate radio module, with direct RF integration deferred until current Israeli
requirements and locally obtainable modules have been researched.

### Exact next learning stage

Begin Stage 6 by separating pure sensor-conversion logic from HAL/peripheral code.
Create a small application-owned C module for ADC-count-to-millivolt and TMP36
temperature conversion, keeping generated files and hardware access separate.
Add host-side unit tests for boundary and representative cases, including raw ADC
counts 0 and 4095 and a temperature below 0 C. Extend GitHub Actions to compile and
run those tests before building/uploading firmware.

Keep the work learning-first: briefly explain why pure functions are testable on
the host, give the user a small implementation task, review it, and then automate
the tests. After this stage, address explicit ADC start/stop/poll error handling
and structured telemetry, followed by bidirectional UART and the laptop
serial-to-MQTT gateway. Then add I2C, SPI, and more sensors before migrating the
telemetry platform to Hetzner. Do not jump directly to DMA, RTOS, RF hardware, or
Hetzner deployment.

### Working-tree caution

`AGENTS.md` contains an unrelated tracked modification. `tmp/`,
`firmware/stm32_telemetry/.settings/`, and
`firmware/stm32_telemetry/.gitignore` are unrelated/untracked local content. Do not
stage, commit, delete, or modify them unless the user explicitly changes this
instruction.

## Codex Progress Update - September 15, 2026

This is the latest and authoritative continuation point. It supersedes every
older next-action section above.

### Stage 6 complete: host-tested sensor conversion logic

ADC and TMP36 arithmetic is separated from HAL/peripheral access into the
application-owned module:

- `firmware/stm32_telemetry/App/Inc/sensor_conversion.h`;
- `firmware/stm32_telemetry/App/Src/sensor_conversion.c`.

The module exposes pure functions for 12-bit ADC-count-to-millivolt conversion
and TMP36 millivolt-to-tenths-Celsius conversion. `main.c` still uses HAL to start,
poll, read, and stop ADC1, but calls this module to interpret the TMP36 reading.
The module has no HAL or MCU dependency and is compiled both into the Arm firmware
and a native host test executable.

`firmware/stm32_telemetry/tests/` contains a standalone CMake/CTest host project.
Six assertions cover ADC counts 0, 2048, and 4095 plus TMP36 results at 0.0 C,
25.0 C, and -10.0 C. GitHub Actions installs native GCC and configures, builds,
and runs these tests before the Release firmware build and packaging steps. A test
failure therefore prevents firmware packaging.

Commit `51cb377` records Stage 6. GitHub Actions run `34960752417` passed the host
tests, Release firmware build, packaging, and artifact upload.

### Stage 7 started: explicit ADC start handling

The first Stage 7 increment is implemented in the `main.c` `USER CODE` region:

- the result of `HAL_ADC_Start()` is stored as `HAL_StatusTypeDef`;
- polling and conversion proceed only after `HAL_OK`;
- a non-OK start emits `ADC error: start failed` over UART;
- `HAL_ADC_Stop()` executes exactly once after every successful start, including
  when either poll fails, and is not called after a failed start;
- the normal two-channel telemetry path is otherwise unchanged.

Host tests still pass. The Debug firmware builds without warnings at 14,836 bytes
flash and 2,136 bytes RAM. The Release firmware builds without warnings at 9,600
bytes flash and 2,136 bytes RAM. This Stage 7 increment has not yet been flashed or
physically exercised on the NUCLEO.

### Exact next learning step

Continue Stage 7. Explain that `HAL_ADC_PollForConversion()` can return timeout or
error even after ADC start succeeds. Give the user one small coding task at a time
to add distinct UART diagnostics for the rank 1/TMP36 poll and rank 2/light poll.
Preserve the single cleanup call after every successful start. Review the user's
code before building. Then store and check the `HAL_ADC_Stop()` result and report
stop failure. After all paths compile, build Debug and Release, flash the NUCLEO,
and verify normal telemetry remains operational. Do not attempt to manufacture a
hardware ADC failure by unsafe wiring.

After Stage 7, define clean structured telemetry, then proceed to bidirectional
UART framing/parsing and the laptop serial-to-MQTT gateway. I2C, SPI, additional
sensors, Hetzner migration, HIL CI, watchdogs/fault handling, Zephyr, secure
releases, and the custom PCB remain later milestones.

### Current hardware state

The most recently verified hardware state remains the September 14 setup: the
NUCLEO-F446RE, TMP36 on A0/PA0, photoresistor divider on A1/PA1, LD2, B1, and
USART2/COM3 were operational. Do not assume COM3 is open or the board remains
connected; inspect before using it.

### Working-tree caution

`AGENTS.md` contains an unrelated tracked modification. `tmp/`,
`firmware/stm32_telemetry/.settings/`, and
`firmware/stm32_telemetry/.gitignore` are unrelated/untracked local content. Do not
stage, commit, delete, or modify them unless the user explicitly changes this
instruction.

## Codex Progress Update - September 17, 2026

This is the latest and authoritative continuation point. It supersedes every
older next-action section above.

### Stage 7 complete: ADC lifecycle diagnostics

The two-rank ADC acquisition path now checks and reports each HAL operation:

- ADC start failure;
- temperature/rank-1 poll failure;
- light/rank-2 poll failure;
- ADC stop failure.

Each poll result is stored as `HAL_StatusTypeDef`. ADC values are read only after
their corresponding polls succeed. `HAL_ADC_Stop()` executes exactly once after
every successful start, including either poll-failure path, and is not called after
a failed start. Successful acquisition still emits one combined temperature/light
telemetry line.

Native sensor-conversion tests pass. Debug and Release firmware builds complete
without warnings. Debug uses 15,008 bytes of flash and 2,136 bytes of RAM; Release
uses 9,724 bytes of flash and 2,136 bytes of RAM.

The Debug ELF was flashed over ST-LINK/SWD and normal operation was physically
verified: TMP36 and light readings arrived approximately every two seconds, and B1
press/release events remained functional. A temporary loss of terminal display was
isolated to the host COM3 session because LD2 continued blinking and closing and
reopening COM3 restored output without a firmware change.

Do not deliberately manufacture ADC failures using unsafe wiring. Error-path
unitability should be improved later by separating acquisition control from direct
HAL calls.

### Exact next learning stage

Begin Stage 8 by defining versioned, newline-delimited JSON telemetry. Preserve
integer temperature representation, bounded `snprintf()`, and one record per sensor
sample. Include at least record type, schema version, uptime, temperature raw count,
nominal-reference millivolts, temperature in tenths Celsius, and light raw count.

After the telemetry record is implemented, built, flashed, and visually verified,
add host-side schema/parsing tests. Then proceed to framed UART receive commands,
malformed-input recovery, and the laptop serial-to-MQTT gateway.

### Stage 8 firmware increment verified

The successful sensor record is now emitted as newline-delimited JSON with schema
version 1, MCU uptime, raw temperature ADC count, nominal-reference millivolts,
temperature in tenths Celsius, and raw light ADC count. The schema constant is used
by the serializer, the output buffer has been checked against the maximum formatted
length, and the existing bounded `snprintf()` check remains in place.

Native tests and both firmware builds pass without warnings. With JSON telemetry,
Debug uses 15,096 bytes of flash and 2,136 bytes of RAM; Release uses 9,780 bytes
of flash and 2,136 bytes of RAM. The Debug firmware was flashed and the user
physically verified valid JSON telemetry on COM3. The next increment is a host-side
parser and tests that accept schema-1 telemetry while rejecting malformed JSON,
missing fields, wrong types, and unsupported schemas.

## Codex Progress Update - September 17, 2026: Host validation complete

The host telemetry validation increment is complete. The new dependency-free
Python gateway parser lives under `gateway/` and returns an immutable, typed
schema-1 record. It performs exact type checks, including rejecting JSON booleans
where integers are required, and does not silently coerce MCU uptime or sensor
values.

Nine standard-library `unittest` tests cover:

- a representative valid schema-1 record;
- malformed JSON;
- missing required fields;
- unsupported schema versions;
- incorrect field types and boolean-as-integer cases;
- preservation of large, zero, and signed integer values;
- non-object JSON;
- explicit rejection and classification of startup, B1, ADC-diagnostic, empty,
  and unknown text lines sharing the UART stream.

GitHub Actions runs the Python parser tests before installing the ARM tools and
before building or packaging firmware. The existing native C tests and firmware
build pipeline remain intact.

### Exact next learning stage

Design and implement a small framed bidirectional UART command protocol. Add one
command at a time, starting with a host-testable frame/parser contract, then cover
timeouts, overflow, and recovery when malformed input is followed by a valid
command. Do not begin the serial-to-MQTT gateway until malformed command recovery
works. Do not require COM3 or connected hardware for the initial protocol tests.

### Working-tree caution

`AGENTS.md` contains an unrelated tracked modification. `tmp/`,
`firmware/stm32_telemetry/.settings/`, and
`firmware/stm32_telemetry/.gitignore` remain unrelated local content and must not
be staged, modified, deleted, or committed without explicit instruction.

## Codex Progress Update - October 11, 2026

The first host-testable inbound command increment is complete and pushed as
commit `86a1542`. The HAL-independent C parser accepts the exact uppercase
`STATUS` frame and rejects null, empty, lowercase, truncated, and newline-bearing
inputs. Native parser tests run through the existing CMake/CTest project, and the
parser also compiles for the Arm firmware target. Debug and Release firmware
builds succeeded, the Debug image was flashed, and reconstructed TMP36 and
photoresistor wiring produced stable, plausible JSON telemetry with normal light
response.

Work has started on a separate HAL-independent byte-stream framer. Its header,
initializer, implementation, and native test are present locally but are not yet
connected to the firmware target or UART. The implemented and tested behavior is:

- initialization resets length and discard state without clearing the buffer;
- ordinary bytes append to the frame and return `COMMAND_FRAME_IN_PROGRESS`;
- carriage return is ignored for CRLF compatibility;
- newline returns `COMMAND_FRAME_READY` without entering the delimiter into the
  payload.

The framer is deliberately incomplete and must not be integrated into UART yet.
It does not protect the 32-byte buffer from overflow, implement discard-until-
newline recovery, handle partial-frame timeouts, or connect completed frames to
`command_parse()`.

### Exact next learning step

Add a native test that fills all `COMMAND_FRAME_CAPACITY` bytes, then pushes one
additional byte and expects `COMMAND_FRAME_OVERFLOW`, unchanged length, and
`discarding == true`. Review the test before changing the implementation. Then
add the bounds check before the buffer write, test discard-until-newline recovery,
and prove that a valid `STATUS` command works immediately after an oversized
frame. Only after those host tests pass should the framer be added to the firmware
target and UART reception begin.

### Working-tree caution

`AGENTS.md`, `docs/`, `research/`, `tmp/`,
`firmware/stm32_telemetry/.settings/`, and
`firmware/stm32_telemetry/.gitignore` are unrelated local content and must not be
staged, modified, deleted, or committed without explicit instruction.

## Codex Progress Update - October 12, 2026

This is the latest and authoritative continuation point. It supersedes older
next-action sections. The session began at `30f54da` (Start host-tested UART
command framing), with local main and origin/main synchronized.

### Command framing: overflow protection and recovery

The user added a native test that initializes a frame, pushes exactly
COMMAND_FRAME_CAPACITY ordinary bytes, checks each result, stored byte, and
length increment, then pushes an extra byte and checks COMMAND_FRAME_OVERFLOW,
unchanged length, and discarding == true. The test was reviewed before the
implementation was changed.

command_frame_push() now checks capacity before writing to the buffer. The
first excess ordinary byte sets discarding and returns COMMAND_FRAME_OVERFLOW.
While discarding, bytes are ignored and return COMMAND_FRAME_IN_PROGRESS;
newline resets length and discard state, also returning IN_PROGRESS. A normal
newline still returns COMMAND_FRAME_READY while preserving the payload and
length. CR remains ignored. The assistant made the final small return-placement
correction at the user's explicit request.

The user explicitly chose to skip additional recovery tests. Recovery behavior
has been reviewed, but discard-until-newline and STATUS immediately after an
oversized frame have NOT been verified by automated tests or physical UART tests.
Do not claim that the existing tests prove recovery. Partial-frame timeouts,
UART reception, frame-to-parser dispatch, and command responses remain unimplemented.

The user added App/Src/command_framer.c to the STM32 target_sources list. Both
Debug and Release builds passed, as reported by the user. The framer is compiled
for STM32 but is not connected to UART or used by main.c yet.

### Validation and execution preference

All three native CTest executables passed after the bounds check (assistant-run).
After the final recovery correction, the user ran the existing native tests and
reported that they passed. The user subsequently ran both firmware builds and
reported that both passed. No firmware was flashed in this session.

From now on, leave ALL test execution, firmware builds, and flashing to the user.
Review code first, then give the exact PowerShell commands and let the user report
results. Do not silently run these actions or fix learning code. Keep the user as
the primary developer, with one small C task at a time and a basic mental model
before implementation. Provide full implementations only when explicitly asked.

### Exact next learning task (assigned, not yet implemented)

In firmware/stm32_telemetry/Core/Src/main.c:

1. Add #include "command_framer.h" in USER CODE BEGIN Includes.
2. Declare CommandFrame command_frame in USER CODE BEGIN 2, before while (1).
3. Call command_frame_init(&command_frame) there once.

These changes are not present at session end. Explain that this persistent frame
must survive loop iterations; resetting it each iteration would erase partial
commands. Review the user's setup, then continue UART reception in small steps.
Inspect the existing loop before choosing a receive approach: it contains blocking
UART transmission, ADC polls, and button-debounce delay, so consider receive data
loss and overrun when designing reception. Do not introduce a full implementation.
A READY payload is parsed with command_parse(frame.buffer, frame.length), then
reset by the caller; the buffer is not a null-terminated C string.

### PowerShell commands

From the repository root, native tests:

```powershell
cmake -S firmware/stm32_telemetry/tests -B firmware/stm32_telemetry/build/host-tests -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build firmware/stm32_telemetry/build/host-tests
ctest --test-dir firmware/stm32_telemetry/build/host-tests --output-on-failure
```

Firmware builds:

```powershell
Push-Location firmware/stm32_telemetry
cmake --preset Debug
cmake --build --preset Debug
cmake --preset Release
cmake --build --preset Release
Pop-Location
```

### Hardware and working-tree caution

The user is packing up the circuit and materials. Do not assume the board is
connected, COM3 available, or the previous wiring intact. The last physical
verification remains October 11: TMP36 on A0/PA0 and the photoresistor divider on
A1/PA1 produced normal JSON telemetry and light response. Recheck physical setup
before future flashing or UART validation.

Preserve and never stage, modify, delete, or commit unrelated AGENTS.md, docs/,
research/, tmp/, firmware/stm32_telemetry/.settings/, and
firmware/stm32_telemetry/.gitignore. Never use git add .
