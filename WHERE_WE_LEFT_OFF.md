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
