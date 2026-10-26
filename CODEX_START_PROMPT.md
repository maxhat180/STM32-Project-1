# Codex Continuation Prompt

Paste the following into a new chat opened in this repository:

---

Continue mentoring me through my STM32 NUCLEO-F446RE environmental-monitor project
in C:\Users\Owner\Documents\Work\Embedded. Read AGENTS.md, CODEX_START_PROMPT.md,
the latest October 26, 2026 STM32 section of WHERE_WE_LEFT_OFF.md,
HARDWARE_INVENTORY.md, firmware/README.md, and relevant current source. Inspect
Git status first and preserve unrelated changes. Older handoff sections may be
stale; use the current code and latest validation evidence.

I am the primary developer, preparing for embedded R&D, hardware validation, and
integration interviews. Explain concepts first, ask me to propose an implementation,
give one small task, and review my code. Keep responses short. Do not change code
without explicit authorization. Leave tests, builds, and flashing to me unless I
ask otherwise. Defer cloud, MQTT, and custom PCB work for now.

Current firmware: CubeMX/HAL C, CMake/Ninja/Arm GCC, TMP36 on A0/PA0, photoresistor
divider on A1/PA1, integer conversion, two-second schema-1 JSON telemetry through
USART2/ST-LINK COM3 at 115200 8-N-1, and LD2/B1 behavior. Native C/Python tests and
GitHub Actions packaging exist. No automated tests or CI status were checked today.

On October 26 I used VS Code with ST's STM32 extension and ST-LINK GDB to program
and debug the Debug ELF. At a breakpoint BEFORE uart_rx_count++ (main.c:289),
sending C gave uart_rx_byte=67 and count=6; continuing and pausing showed count=7.
No second receive breakpoint followed C. Earlier extra increments are unexplained;
no specific line-ending mode was established. This verifies a controlled character
through reception, callback, counting, and rearming, not sustained reliability.
I learned the vector/IRQ/HAL/weak-callback path and basic WATCH/pause/continue.
Give concrete debugger UI steps when needed.

The callback stores each incoming byte into the same uart_rx_byte, counts it, and
rearms HAL_UART_Receive_IT for one byte. No receive queue exists. CommandFrame is
already included and initialized once before the main loop. The pure framer has
32-byte capacity, CR ignore, newline completion, overflow rejection, and discard
until newline. Existing tests cover overflow but dedicated recovery tests were
previously skipped. The parser accepts exact uppercase STATUS using explicit
length, without null termination. Neither module is connected to UART yet.
Command responses, UART-error recovery, partial-frame timeouts, and sustained
receive validation are unfinished.

Next: explain a small bounded receive buffer shared between interrupt producer
and main-loop consumer. Ask me to propose a design before implementation. Inspect
blocking UART sends, ADC polling, and button debounce delays. Discuss ownership,
indices, full/empty conditions, overflow policy, and why volatile alone is not
synchronization. Keep ISR work short and progress toward STATUS in reviewed steps.
Do not generate the full implementation.

Unresolved measurement issue: DMM showed TMP36 generally near 0.735-0.739 V while
UART ADC readings sometimes jumped (one set: 668-821 mV). Supply measured 3.31 V;
firmware assumes 3.30 V, which does not explain the excursions. Grounding A0 gave
about 0-1 mV. Reconnecting the sensor restored excursions. Adding 100 nF ceramic
across TMP36 supply/GND did not resolve them. Grounding A1 produced light_raw 0-4
but temperature still varied 715-751 mV over ten samples. Nucleo-ground versus
sensor-ground DMM measurement showed 0 V. DMM averaging may hide transients;
root cause and ADC scan/poll behavior remain unproven. Do not hide it with averaging
or claim calibrated accuracy. Office AC may change the temperature baseline.

I now have banana-plug clip leads for my DMM and use breadboard jumpers as clip
contacts. Before session close I was instructed to restore divider junction to A1
after removing the A1-GND test jumper, remove the breakpoint, stop debugging,
close COM3, and unplug USB. Those physical actions were not confirmed. Check the
current connection/wiring state rather than assuming it.

Start with a brief UART milestone recap, then explain why bytes need buffering
before the main loop can assemble a command. Do not repeat completed setup unless
needed. Preserve all unrelated local changes; do not stage or commit automatically.

---
