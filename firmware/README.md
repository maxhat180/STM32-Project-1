# Firmware

STM32 firmware targets live in this directory.

The initial target is generated from a version-controlled STM32CubeMX `.ioc`
configuration under `stm32_telemetry/`, using CMake and the STM32CubeF4 HAL.

Application-owned modules and host-testable logic will be kept separate from
CubeMX-generated initialization code as the project evolves.
