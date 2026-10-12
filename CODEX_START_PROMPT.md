# Codex Continuation Prompt

Paste the following into a new chat opened in this repository:

---

Continue mentoring me through the STM32 environmental-monitor project in this
repository. First read AGENTS.md, PROJECT_CONTEXT.md,
PROJECT_IDEAS_AND_ROADMAP.md, HARDWARE_INVENTORY.md, CODEX_START_PROMPT.md, and
especially the latest October 12, 2026 section of WHERE_WE_LEFT_OFF.md. Inspect
the current Git status and code before making changes; the repository is authoritative.

We completed overflow protection for the HAL-independent command framer:
- a 32-byte buffer with explicit length and discard state;
- native tests fill every slot and verify acceptance, contents, length, and overflow;
- the capacity check occurs before the buffer write;
- overflow preserves length, sets discarding, and returns COMMAND_FRAME_OVERFLOW;
- discarded bytes return IN_PROGRESS; newline resets the rejected frame;
- normal newline returns READY while preserving payload; CR is ignored.

The framer now compiles into the STM32 firmware target, but UART reception,
frame-to-parser dispatch, command responses, and partial-frame timeouts are not
implemented. The existing parser recognizes only exact uppercase STATUS using
explicit buffer length, with no requirement for null termination.

I chose to skip extra recovery tests. Recovery has been reviewed, but the existing
tests do not prove discard recovery or STATUS immediately after an oversized frame.
The existing three native CTest executables passed, and I reported both Debug and
Release firmware builds passing. Nothing was flashed in this session.

The exact next task was assigned but is not implemented yet:
1. In main.c USER CODE BEGIN Includes, include command_framer.h.
2. In USER CODE BEGIN 2, declare CommandFrame command_frame before while (1).
3. Initialize it once with command_frame_init(&command_frame).
Explain persistent state across loop iterations, then review my code before
continuing with UART reception in small steps. Inspect existing blocking UART
transmission, ADC polling, and debounce delays before choosing a receive approach.

I am the primary developer. Explain the basic mental model, give me small C tasks,
and review my code before changing it. Do not provide a full implementation unless
I ask. Leave all test execution, firmware builds, and flashing to me; give exact
PowerShell commands, then wait for my results. Do not run those actions yourself.

The last physical verification was October 11: NUCLEO-F446RE with TMP36 on A0/PA0,
photoresistor divider on A1/PA1, and schema-1 newline-delimited JSON telemetry over
USART2/ST-LINK VCP COM3 at 115200 8-N-1. I packed up the circuit and materials, so
do not assume the board is connected or wiring intact. Verify voltage requirements
and exact pin mappings before giving future wiring instructions.

Preserve and never stage, modify, delete, or commit unrelated AGENTS.md, docs/,
research/, tmp/, firmware/stm32_telemetry/.settings/, and
firmware/stm32_telemetry/.gitignore. Never use git add .

---
