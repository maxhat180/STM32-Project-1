# STM32 Environmental Monitor

An STM32 sensor node with embedded C firmware, host-side telemetry validation,
and automated firmware builds and deployment artifacts. The current hardware
uses a NUCLEO-F446RE, a TMP36 temperature sensor, and a photoresistor divider.

```text
TMP36 + photoresistor -> STM32 ADC -> JSON over USART2 / ST-LINK USB -> host
```

## Implemented capabilities

- LD2 status indication and debounced B1 input events.
- Two-channel ADC acquisition with explicit start, poll, and stop diagnostics.
- Integer temperature conversion and versioned newline-delimited JSON telemetry.
- HAL-independent sensor conversion, command parsing, and command framing modules
  with native C tests.
- A Python schema-validation boundary with tests for malformed and incompatible
  telemetry.
- GitHub Actions validation and Release cross-compilation, with checksummed
  ELF/BIN/HEX deployment packages and an ST-LINK flashing helper.
- Initial interrupt-driven UART reception that counts incoming bytes and rearms
  reception after each byte; Debug and Release builds previously passed. A manual
  debugger test on October 26 verified reception of ASCII C (67), callback
  execution, counting, and continued reception. Buffered command handling and
  sustained receive reliability have not been verified.

Sensor telemetry and programming a CI-built artifact have been physically
verified. Intermittent temperature ADC excursions relative to DMM readings remain
under investigation; calibrated accuracy is not established.
Bidirectional command handling is incomplete: reception does not yet
buffer bytes or dispatch frames to the parser, and UART-error recovery and
partial-frame timeouts remain outstanding. Framer overflow rejection is covered
by native tests; discard-until-newline recovery is implemented but has no
dedicated recovery test.

## Repository layout

| Path | Purpose |
|---|---|
| `firmware/stm32_telemetry/` | CubeMX configuration, HAL firmware, application modules, and native C tests |
| `firmware/nano_ble/` | EverCold Arduino Nano 33 BLE Sense Rev2 analog/BLE prototype and manual setup |
| `gateway/` | Python BLE receiver, telemetry parser, and parser unit tests |
| `.github/workflows/` | Firmware validation, build, and artifact packaging |
| `tools/` | Windows firmware flashing helper |
| `HARDWARE_INVENTORY.md` | Development hardware and sensor inventory |
| `CUSTOMER_SOLUTION_BRIEF.md` | Proposed use case, requirements, and engineering constraints |

See [firmware/README.md](firmware/README.md) for build commands and validation
status.

## Planned extensions

- A NUCLEO-connected temperature/light sensor PCB, followed by a standalone MCU
  board with power, programming, and test access.
- Buffered UART commands with error recovery and timeouts.
- A local serial-to-MQTT gateway and broker, followed by server-side storage and
  dashboards.
- Digital sensor interfaces, static analysis, hardware-in-the-loop testing, and
  release integrity checks.

These extensions are planned; they are not implemented capabilities.

## Gateway telemetry validation

The host-side gateway begins with a dependency-free Python validation boundary for
the mixed UART stream. It accepts schema-1 JSON telemetry, preserves the MCU's
integer values without coercion, rejects malformed or incompatible records, and
classifies the firmware's startup, button, and ADC diagnostic text.

Run its tests from the repository root:

```powershell
py -m unittest discover -s gateway/tests -p "test_*.py" -v
```

GitHub Actions runs these tests before building and packaging firmware.

## Firmware CI artifacts

GitHub Actions builds the Release firmware for every push to `main`, every pull
request, and every manual workflow run. A successful run publishes a directly
downloadable artifact named like:

```text
stm32-telemetry-20260914-153045-utc.zip
```

The timestamp is UTC. The package contains ELF, BIN, Intel HEX, linker map,
memory-usage, build-provenance, and SHA-256 checksum files.

### Flash the newest downloaded artifact on Windows

Connect the NUCLEO-F446RE by USB and download the workflow artifact into the
standard Windows Downloads folder. From the repository root, run exactly one
command:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-firmware.ps1
```

The helper finds the newest `stm32-telemetry-*-utc.zip` in Downloads, extracts
it to a uniquely named temporary directory, verifies the Intel HEX SHA-256,
flashes it with STM32CubeProgrammer over ST-LINK/SWD, verifies the write, resets
the MCU, and removes only that temporary directory.

To flash a specific downloaded package instead, pass its absolute path:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-firmware.ps1 `
  -ArtifactPath 'C:\Users\Owner\Downloads\stm32-telemetry-YYYYMMDD-HHMMSS-utc.zip'
```
