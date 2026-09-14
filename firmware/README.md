# Firmware

STM32 firmware targets live in this directory.

The initial target is generated from a version-controlled STM32CubeMX `.ioc`
configuration under `stm32_telemetry/`, using CMake and the STM32CubeF4 HAL.

Application-owned modules and host-testable logic will be kept separate from
CubeMX-generated initialization code as the project evolves.

## Current target

The NUCLEO-F446RE firmware currently provides:

- LD2/B1 GPIO behavior and debounced button telemetry;
- USART2 output through the onboard ST-LINK Virtual COM Port;
- two-channel ADC1 polling for a TMP36 on PA0 and photoresistor divider on PA1;
- integer temperature conversion and two-second sensor telemetry.

## Build

From `firmware/stm32_telemetry`:

```powershell
cmake --preset Debug
cmake --build --preset Debug
```

GitHub Actions also builds the Release preset and publishes UTC-timestamped ZIP
artifacts containing flashable ELF, BIN, and Intel HEX images plus checksums and
build metadata. See the root `README.md` for the one-command Windows flashing
workflow.
