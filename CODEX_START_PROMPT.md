# Codex Continuation Prompt

Paste the following into a new Codex chat opened in this project folder:

---

Continue mentoring me through the STM32 environmental-monitor project in this
repository. First read `AGENTS.md`, `PROJECT_CONTEXT.md`,
`PROJECT_IDEAS_AND_ROADMAP.md`, `HARDWARE_INVENTORY.md`, and especially the latest
dated section of `WHERE_WE_LEFT_OFF.md`. Inspect the current files before proposing
changes; the repository is authoritative if this prompt differs from it.

The primary board is a NUCLEO-F446RE with an STM32F446RET6. Firmware is the
CubeMX-generated STM32CubeF4 HAL C project under `firmware/stm32_telemetry`, built
with CMake, Ninja, and GNU Arm GCC. USART2 uses PA2/PA3 through the onboard ST-LINK
Virtual COM Port on COM3 at 115200 8-N-1. LD2 is PA5 and B1 is active-low on PC13.

Stages 0, 1, and 2 are complete. LD2 toggles every 250 ms while B1 is released,
stays on while B1 is held, and UART reports startup plus B1 transitions. Stage 2
added a non-blocking two-second UART schedule. The build/flash/UART path is working.
VS Code go-to-definition remains broken despite the installed C/C++ and STM32Cube
clangd extensions, but that is not blocking the project.

Stage 3 ADC bring-up is in progress. A TMP36GZ is currently wired to NUCLEO 3V3,
A0/PA0, and GND. The board is connected and powered. The user verified that the
sensor does not heat and that the previous firmware behavior remains intact. ADC1
was intentionally simplified in CubeMX to one PA0 channel: 12-bit, single
software-triggered conversion, scan/continuous/DMA disabled, PCLK2 divided by 4,
and 84-cycle sampling time.

`main.c` now samples ADC1 every two seconds with the polling HAL API, formats
`ADC raw=<value>` using a bounded `snprintf()` buffer, and sends it through USART2.
The Debug build succeeds without warnings at 14,548 bytes flash and 2,136 bytes
RAM. The firmware was flashed and verified. A direct COM3 capture produced stable
raw readings around 868-881 counts, centered near 875, which is plausible for room
temperature.

Begin by briefly explaining how a 12-bit ADC raw count converts to millivolts and
how the TMP36's 500 mV offset and 10 mV/C slope convert voltage to Celsius. Then
give me one small integer-math coding task to extend the existing UART line with
voltage and temperature. Use a signed type for temperature, avoid floating-point
`printf`, and review my code before asking me to build or flash it. Afterward,
physically verify that warming the sensor increases the reported value.

This is a hands-on embedded C learning project. Work in very small stages, explain
concepts before implementation, let me write important code, and review mistakes
rather than silently fixing them unless I explicitly ask. Keep application edits
inside CubeMX `USER CODE` regions. Do not add PA1/photoresistor, DMA, CI/CD, or
server work until the Stage 3 temperature-conversion check is complete.

Preserve unrelated working-tree changes: `AGENTS.md` and `tmp/`. Also leave the
currently untracked local IDE metadata under `firmware/stm32_telemetry/.settings/`
and its local `.gitignore` alone unless we deliberately decide to adopt them.

---
