# UART STATUS checkpoint

## STM32 checkpoint - November 2, 2026

This supersedes older STM32 continuation instructions. Nano work is separate.

### Hardware validation completed by the user

NUCLEO-F446RE, COM3, 115200 8-N-1, no flow control, PowerShell SerialPort.
The user supplied board output; the assistant did not independently operate hardware.
- STATUS accepted with LF and CRLF; invalid HELLO rejected, then STATUS succeeded.
- Write("STA") produced no command reply; WriteLine("TUS") completed STATUS.
- A 33-byte A payload produced ERR command too long; subsequent STATUS worked.
- Twenty STATUS commands paced with 100 ms sleeps produced twenty replies, all
  rx_dropped=0 and uart_errors=0; telemetry continued. This is a short paced test,
  not a sustained-throughput guarantee.
- A 200-byte A burst produced a frame-length error; subsequent STATUS at uptime
  2046579 reported rx_dropped=0 uart_errors=0. Queue overflow was NOT induced.
- VS Code STM32/ST-LINK debug programmed firmware/stm32_telemetry/build/Debug/
  stm32_telemetry.elf and stopped at main.c:143 before HAL_Init. User continued,
  reopened COM3, paused CPU, sent STATUS while halted, then resumed. Initial
  confusion left CPU paused longer; queued host serial text included pre-reset
  telemetry and a partial line. These are not clean post-reset measurement logs.
- After resume: ERR RX loss; send newline then retry. A fresh LF followed by
  STATUS yielded uptime_ms=34377 rx_dropped=0 uart_errors=1 schema=1. Another
  STATUS at 77865 ms kept those counters unchanged; telemetry continued.
  One debugger-induced UART receive-loss recovery case is verified. Hardware
  overrun is inferred, not proven: SR/ErrorCode were not captured. The counter
  also includes failed receive rearming. Software-queue overflow remains untested
  on hardware. GPIO regression, long-duration load and partial-line timeout remain.

No firmware source edits or new automated build/test runs occurred this session.
Historical October 27 records report five C test executables, nine Python tests,
Debug/Release builds passing. Earlier GitHub CI at a90b228 passed but did not cover
then-uncommitted command integration. The session save includes that integration
and its tests so it is reproducible from GitHub.

Software checks passed locally on October 27, 2026. The user subsequently supplied
COM3 board output verifying the basic command cases below; the assistant did not
operate or independently test the hardware.

## User-reported board results - October 27, 2026

- Two consecutive STATUS commands produced two STATUS OK responses at uptime
  2449837 and 2449843 ms.
- HELLO followed by STATUS produced ERR invalid command, then STATUS OK at
  2451858 ms.
- The supplied slow 33-character test produced ERR command too long, then a
  subsequent STATUS produced STATUS OK at 2455937 ms.
- Sensor JSON continued between the responses. Every displayed STATUS reported
  rx_dropped=0 and uart_errors=0.
- An earlier explicit-LF PowerShell test also produced STATUS OK. VS Code
  keystroke entry had shown no response; a CR-only line ending remains the likely
  explanation, not a captured-byte measurement.

These results establish basic command handling and overlong-frame recovery, not
software receive-queue overflow, UART-fault recovery, sustained throughput, CRLF
acceptance on hardware, GPIO regression checks, or temperature accuracy. Those
hardware checks remain unverified.

The user independently added schema=1 to the STATUS response and supplied new
board output: consecutive responses at 26372/26378 ms, invalid-command recovery
at 28409 ms, and overlong-frame recovery at 32462 ms. All four STATUS responses
contained schema=1 with zero rx_dropped and uart_errors, and sensor JSON continued.
The user identified newline as the frame boundary; mentoring clarified that
resetting the frame length prevents the next command appending to the previous one.
The independent code-change and basic frame-reset learning checkpoint are complete;
receive-loss resynchronization remains a separate topic to review.

## Protocol

USART2 uses 115200 baud, 8 data bits, no parity, one stop bit, no flow control.
Commands are case-sensitive ASCII lines terminated by LF or CRLF. CR alone does
not finish a line. Maximum payload is 32 bytes; the receive queue holds 63 bytes.
Empty lines, unknown words, embedded CR, and non-printable bytes are invalid.

Sending `STATUS` followed by LF should produce a line such as:

```text
STATUS OK uptime_ms=12345 rx_dropped=0 uart_errors=0 schema=1
```

Values shown are an example, not an observed hardware result. STATUS OK means the
command was recognized; it does not certify sensor health or measurement accuracy.
`rx_dropped` counts software queue rejections. `uart_errors` counts HAL UART error
callbacks and failed receive rearming attempts, not an exact number of lost bytes.
Startup text, button events and two-second sensor JSON continue on the same UART.
The Python telemetry validator classifies command replies as OTHER_TEXT.

Unknown commands produce `ERR invalid command`. A 33rd payload byte produces
`ERR command too long`, then the framer ignores the remainder through LF.
Software queue overflow or a reported UART error produces
`ERR RX loss; send newline then retry`. Queued bytes are discarded conservatively;
after seeing this error, send a fresh LF, then send STATUS again. Already queued
valid commands may be discarded during this recovery.

There is no flow control or partial-line timeout. Arbitrarily fast sustained
traffic is not guaranteed: ADC polls, debounce and main-loop UART TX still block.

## Manual board checklist

1. Use the normal manual Debug build/flash workflow for the current source;
   do not flash an older downloaded CI artifact. The locally checked Debug ELF is
   `build/uart-buffer-arm-debug/stm32_telemetry.elf` relative to the repository root.
2. Open the board's current COM port (previously COM3), using the settings above
   and LF or CRLF line endings. Close other serial programs first. Remove receive
   breakpoints: pausing the MCU while sending can itself cause UART overruns.
3. Reset. Check startup text, JSON approximately every two seconds, LED blinking,
   and B1 hold/release behavior.
4. Send STATUS with a line ending. Expect exactly one STATUS OK response. Send
   two STATUS lines together and expect two responses with increasing/nondecreasing
   uptime values. Try both LF and CRLF.
5. Send `status`, `HELLO` and an empty line separately. Expect invalid-command
   errors. A following STATUS must still work.
6. Send this 33-character line at a human typing pace, then Enter:
   `AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`. Expect one too-long error. Then send
   STATUS and expect recovery. Slow entry separates frame overflow from queue overflow.
7. Optional debugger overflow check: break at UART_ProcessCommands entry in the
   main loop, send more than 63 characters while halted, remove the breakpoint,
   then continue. Expect an RX-loss error; send a fresh newline and then STATUS.
   Halting can cause hardware overrun as well as software loss, so this does not
   isolate queue overflow. Deterministic software-overflow coverage is in host tests.
8. Confirm telemetry/LED/button operation continues after these checks. Record
   actual output and failures; do not claim sustained traffic or accuracy from this test.

If no response appears, check the COM port and LF setting first. If needed, stop
sending and set a breakpoint in HAL_UART_RxCpltCallback to inspect uart_rx_byte
and uart_rx_count; then remove it before testing complete lines. Inspect
commands.rx.head/tail, commands.dropped, uart_rx_errors and uart_rx_restart_pending.

## Learning checkpoint

Completed: frame-reset explanation and independent schema=1 response addition,
verified by the user on COM3 with the sensor JSON format unchanged. Still to review:
why command_service_poll discards through a newline after receive loss.
