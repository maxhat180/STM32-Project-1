# Firmware

## Latest hardware checkpoint - November 2, 2026

User-supplied COM3 output verified LF/CRLF STATUS, fragmented commands,
invalid/overlong-line recovery, twenty commands paced with 100 ms sleeps, and
one debugger-induced UART receive-loss recovery. After a fresh LF, STATUS reported
rx_dropped=0 uart_errors=1; a later STATUS kept those counters unchanged and
telemetry continued. The exact UART error flag was not captured. A 200-byte
burst caused frame overflow, not software-queue overflow. Sustained throughput,
GPIO regression and software-queue overflow remain unverified on hardware.
See [COMMANDS.md](stm32_telemetry/COMMANDS.md) for evidence and limitations.
No new automated tests/builds were run in this session; October 27 results below
are historical. Next: investigate ADC scan/poll sequencing and the unresolved
TMP36/DMM discrepancy before expanding functionality.

Firmware targets live in this directory, separated by board.

The Nano BLE sensor prototype is under `nano_ble/`; see its
[README](nano_ble/README.md) for manual upload and laptop receiver instructions.
The sections below describe the STM32 target.

The initial target is generated from a version-controlled STM32CubeMX `.ioc`
configuration under `stm32_telemetry/`, using CMake and the STM32CubeF4 HAL.

Application-owned modules and host-testable logic will be kept separate from
CubeMX-generated initialization code as the project evolves.

## Current target

The NUCLEO-F446RE firmware currently provides:

- LD2/B1 GPIO behavior and debounced button telemetry;
- USART2 output through the onboard ST-LINK Virtual COM Port;
- two-channel ADC1 polling for a TMP36 on PA0 and photoresistor divider on PA1;
- integer temperature conversion and two-second, schema-1 JSON sensor telemetry;
- pure sensor-conversion, exact-length STATUS parsing, and command framing modules;
- buffered USART2 interrupt reception and main-loop STATUS responses.

## Current command checkpoint - October 27, 2026

The receive buffer now feeds the existing framer and parser through a small
HAL-independent command service. STATUS responses and errors are transmitted only
from the main loop. Receive loss flushes queued data and requires a fresh newline
before accepting another command. UART errors request main-loop receive recovery.
Sensor acquisition, periodic JSON, LED and button logic were preserved.

Verified locally: all five native C test executables and all nine Python parser
tests passed; GNU Arm GCC 14.3.1 Debug and Release builds passed. Release reports
12,148 bytes flash and 2,280 bytes RAM. Host tests cover buffer limits, frame limits,
overflow/discard recovery, malformed commands, fragmented and consecutive STATUS
commands. They do not verify HAL fault recovery, interrupt timing, physical UART
reliability or hardware behavior. The assistant did not flash or test the board.

The user subsequently supplied COM3 output verifying two consecutive STATUS
responses, invalid-command recovery and slow 33-character overlong-frame recovery.
Telemetry continued, and all displayed responses reported zero rx_dropped and
uart_errors. Receive-queue overflow, UART-fault recovery and sustained traffic
remain unverified on hardware. See COMMANDS.md for the recorded evidence and limits.

See [COMMANDS.md](stm32_telemetry/COMMANDS.md) for expected responses and checks.
Partial-frame timeouts and the USART2 NVIC entry in CubeMX remain outstanding.

## Historical validation status - October 26, 2026

Debug and Release builds previously passed after the interrupt-reception changes,
as reported by the developer. On October 26, the developer launched the Debug ELF
through VS Code/ST-LINK, programmed the board, and verified UART reception with a
breakpoint before the receive counter increment: sending C produced byte 67 and
counter 6; after continuing, the counter was 7. Reception had already handled
earlier bytes, confirming receive rearming. This was a manual character test,
not a sustained traffic, error-recovery, or command-response test.

Sensor JSON telemetry was also observed on COM3. Intermittent temperature ADC
excursions remain unexplained despite a comparatively stable DMM reading. A0
grounding produced about 0-1 mV; adding 100 nF supply bypassing did not resolve
the excursions. Calibration and reliable temperature accuracy are not established.

The command frame is initialized once before the main loop. Received bytes are
currently counted without being queued or passed to the framer. Buffering,
frame-to-parser dispatch, command responses, UART-error recovery, and partial-frame
timeouts are not implemented. The interrupt configuration is currently in protected
user-code sections; the CubeMX `.ioc` does not yet configure the USART2 NVIC entry.

The three existing native C test executables passed at the previous framer
checkpoint; they were not rerun for this interrupt change. They cover frame
initialization, ordinary bytes, delimiters, and capacity/overflow rejection, but
do not establish discard-until-newline recovery or a valid command after overflow.

## Build

### Earlier receive-buffer checkpoint - October 27, 2026

A HAL-independent `uart_rx_buffer` module now provides initialization, push, and
pop operations. Its 64-byte array holds up to 63 unread bytes; a full buffer
rejects the newest byte. Lock-free C11 atomic indices support one interrupt
producer and one main-loop consumer. Initialization must happen before use.

All four native C test executables passed locally, including new checks for
empty/full behavior, byte order, wraparound, overflow rejection and reuse.
A fresh ARM Debug firmware build also passed with GNU Arm GCC 14.3.1.
These host tests do not establish interrupt timing or concurrent hardware behavior.
At that checkpoint the module was compiled but not yet connected to UART reception
or command handling; the current command checkpoint above supersedes that limit.
No firmware was flashed and no hardware test was performed for this checkpoint.

From `firmware/stm32_telemetry`:

```powershell
cmake --preset Debug
cmake --build --preset Debug
cmake --preset Release
cmake --build --preset Release
```

## Native C tests

From the repository root, using a native C compiler, CMake, and Ninja:

```powershell
cmake -S firmware/stm32_telemetry/tests -B firmware/stm32_telemetry/build/host-tests -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build firmware/stm32_telemetry/build/host-tests
ctest --test-dir firmware/stm32_telemetry/build/host-tests --output-on-failure
```

GitHub Actions also builds the Release preset and publishes UTC-timestamped ZIP
artifacts containing flashable ELF, BIN, and Intel HEX images plus checksums and
build metadata. See the root `README.md` for the one-command Windows flashing
workflow.
