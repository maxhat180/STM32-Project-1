# STM32 continuation prompt - November 2, 2026

Continue mentoring me in this repository. Read AGENTS.md, the November 2 STM32
checkpoint at the top of WHERE_WE_LEFT_OFF.md, firmware/stm32_telemetry/COMMANDS.md,
firmware/README.md and current source. Inspect Git status and preserve unrelated work.

I am the primary developer. Keep explanations brief, give one small exercise at a
time, and do not edit learning firmware without explicit permission. Give PowerShell
commands one line at a time, waiting for my results. Prefer PowerShell SerialPort
for COM3 at 115200 8-N-1, no flow control, LF/CRLF. VS Code STM32 extension is the
current debug workflow. Do not bury explanations in HAL internal call chains.

UART commands and receive-loss recovery have user-supplied hardware evidence:
LF/CRLF STATUS, invalid and overlong recovery, fragmented STATUS, twenty paced
commands, and one debugger-induced fault followed by fresh-LF/STATUS recovery.
Final STATUS: uptime_ms=77865 rx_dropped=0 uart_errors=1 schema=1. Telemetry
continued. Exact hardware error flags were not captured; overrun is inferred.
The 200-byte burst did NOT overflow the software queue. Long-duration throughput,
GPIO regression and partial-line timeout remain unverified/unimplemented as noted.
No new builds/tests were run during the November 2 mentoring session.

NEXT: diagnose unresolved temperature ADC excursions. First task already assigned,
not answered: inspect MX_ADC1_Init in Core/Src/adc.c and identify which channels
have ranks 1 and 2. Then explain scan conversion and trace the two polling/read
operations in main.c. Treat sequencing as a hypothesis; establish expected and
observed behavior before changing code. No simultaneous DMM reading today; do not
claim calibrated temperature or hide the problem with averaging.

After ADC diagnosis: nonblocking timing/state machines, timers/ADC/DMA, I2C, SPI,
Python hardware validation and CI/demo polish. Defer MQTT/cloud/PCB work. Check
current bench state: last confirmed CPU running under debugger and COM3 open;
shutdown was not confirmed. Keep Nano progress separate.
