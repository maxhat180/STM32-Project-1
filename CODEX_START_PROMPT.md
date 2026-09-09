# Codex Continuation Prompt

Paste the following into a new Codex chat opened in this project folder:

---

Continue mentoring me through the STM32 environmental-monitor project in this
repository. First read `AGENTS.md`, `PROJECT_CONTEXT.md`,
`PROJECT_IDEAS_AND_ROADMAP.md`, `HARDWARE_INVENTORY.md`, and especially the latest
dated section of `WHERE_WE_LEFT_OFF.md`. Inspect the current files before proposing
changes; do not assume this prompt is more current than the repository.

The primary board is a NUCLEO-F446RE with an STM32F446RET6. The project is a
CubeMX-generated STM32CubeF4 HAL C project under `firmware/stm32_telemetry`, built
with CMake, Ninja, and GNU Arm GCC. USART2 uses PA2/PA3 through the ST-LINK Virtual
COM Port on COM3 at 115200 8-N-1. LD2 is PA5, B1 is PC13 and reads active-low, and
ADC1 channels PA0/A0 and PA1/A1 are configured but not yet used by application
code.

Stage 0 and Stage 1 are complete. I personally reviewed the generated structure,
built and flashed the firmware, and tested it physically. Current behavior:

- LD2 toggles every 250 ms while B1 is released;
- LD2 stays steadily on while B1 is held;
- UART prints startup, B1-pressed, and B1-released messages;
- COM3, ST-LINK/SWD, build, flash, reset, LED, and button have all been verified;
- the latest Debug build uses 10,864 bytes of flash and 1,728 bytes of RAM.

The current board is connected and powered. No external sensors are wired. The
next stage is Stage 2, UART. Begin by briefly explaining the existing UART path
from the MCU through ST-LINK to COM3. Then give me one small coding task to attempt
myself. Review my code before asking me to build or flash it.

This is primarily a hands-on embedded C learning project. Work in very small
stages, explain concepts before implementation, let me write important code, and
review compiler errors or bugs rather than silently fixing them. Do not generate
the whole project, skip ahead to ADC, add CI/CD yet, or aggressively refactor
CubeMX-generated files. Keep application edits inside CubeMX `USER CODE` regions
until we intentionally introduce separate modules.

Preserve unrelated working-tree changes, particularly `AGENTS.md` and `tmp/`.

---
