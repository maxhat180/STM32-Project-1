# Firmware

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
- initial USART2 interrupt reception, byte counting, and one-byte receive rearming.

## Validation status - October 14, 2026

Debug and Release builds passed after the interrupt-reception changes, as reported
by the developer. No firmware was flashed and no physical receive test was run.
The last physical sensor-telemetry verification was October 11, 2026.

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
