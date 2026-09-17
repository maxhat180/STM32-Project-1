# Project Context

## Purpose

This project exists to combine two parts of the user's background:

1. Current DevOps / cloud / automation experience.
2. Electrical Engineering background and renewed interest in embedded development.

The goal is not merely to learn Arduino programming.

The goal is to create a portfolio-quality embedded + DevOps system that demonstrates:

- embedded C
- STM32 development
- physical sensor integration
- firmware architecture
- automated firmware builds
- CI/CD
- hardware-in-the-loop testing
- telemetry
- containerized server infrastructure
- observability
- DevSecOps practices

## Available Hardware

The inventory has been captured and includes the NUCLEO-F446RE, Arduino Uno Rev3,
SparkFun Essential Sensor Kit V2, Seeed Sidekick Basic Kit V2, breadboards,
jumpers, and sensor components. See `HARDWARE_INVENTORY.md` for the authoritative
component-level record.

## Primary Board

Use the NUCLEO-F446RE as the main embedded target.

The Arduino can later be used for:
- quick sensor verification
- simulating another embedded device
- acting as a hardware test partner
- generating known input signals
- helping with hardware-in-the-loop tests

## Preferred Project Direction

Start with a multi-sensor STM32 device.

Initial shape:

Sensors
  |
  v
NUCLEO-F446RE
  |
  | UART / USB serial
  v
Host/gateway
  |
  v
Hetzner
  |
  +--> MQTT
  +--> API
  +--> PostgreSQL
  +--> dashboard

The first milestone should work even before every future component exists.

The network path should be developed locally before depending on the remote
server:

```text
STM32 sensors
  |
  | USB UART
  v
Python gateway on the laptop
  |
  | MQTT
  v
Mosquitto in Docker on the laptop
  |
  v
subscriber / storage / dashboard
```

Once this path is understood and reliable, the broker and downstream services can
move to the existing Hetzner server.

## Required Embedded Coverage

The project should deliberately include, through real increments rather than
isolated toy repositories:

- deeper bidirectional UART work, including framing, parsing, commands, timeouts,
  and malformed-message handling;
- I2C configuration, register transactions, addressing, ACK/NACK behavior, and an
  actual sensor driver;
- SPI configuration, chip-select handling, clock polarity/phase, full-duplex
  transfers, and an actual sensor or peripheral driver;
- additional sensors chosen from the existing inventory where practical;
- debugging of physical buses with evidence-driven tests.

## Long-Term Custom Hardware Goal

The eventual dream project is a compact custom PCB containing:

- a microcontroller;
- a temperature sensor;
- power regulation and protection;
- programming/debug access;
- a reliable timestamp source;
- an optional Bluetooth or LTE communications module;
- the antenna clearance, connectors, and enclosure considerations required by the
  selected transport.

The board should send timestamped temperature data to a remote service, most
likely with MQTT or HTTPS. Bluetooth normally implies a nearby phone or gateway;
LTE can reach the service independently but brings greater power, antenna, SIM,
certification, and sourcing complexity.

RF-capable parts are a project constraint because importing them into Israel may
involve difficult customs review and fees. Do not let this block the firmware,
gateway, or first custom-PCB revision. Keep the application independent of its
transport and prefer a modular architecture:

```text
sensor application -> telemetry interface -> UART / Bluetooth / LTE transport
```

An early custom board may place the MCU, temperature sensor, power, and debug
hardware on the main PCB while exposing power and UART/SPI through a connector for
a separately sourced radio module. When RF hardware enters the active milestone,
verify current Israeli import/regulatory requirements and locally available or
pre-approved modules before selecting parts or finalizing the layout.

## Current Progress - September 17, 2026

The first physical firmware milestone is operational:

- CubeMX/HAL C firmware builds with CMake, Ninja, and GNU Arm GCC;
- LD2 output and active-low B1 input work with debounced UART events;
- USART2 telemetry reaches Windows through the onboard ST-LINK VCP on COM3;
- a TMP36 on PA0 reports integer-converted temperature telemetry;
- a photoresistor divider on PA1 reports raw light telemetry;
- both ADC channels have been physically stimulated and verified;
- GitHub Actions builds Release firmware and publishes UTC-timestamped deployment
  ZIPs containing ELF, BIN, HEX, maps, provenance, and checksums;
- a downloaded CI artifact has been checksum-verified and flashed successfully to
  the physical NUCLEO using the repository's one-command PowerShell helper.
- ADC-count-to-millivolt and TMP36 conversion logic now lives in an
  application-owned, HAL-independent C module;
- native host tests cover ADC boundaries, a representative midpoint, zero and
  representative TMP36 temperatures, and a below-zero case;
- GitHub Actions runs those host tests as a required gate before building and
  packaging Release firmware;
- ADC start, rank-specific poll, and stop results are checked and reported while
  preserving exactly one stop call after every successful start;
- successful sensor samples are emitted as versioned, newline-delimited JSON with
  uptime, raw ADC values, nominal-reference millivolts, and integer temperature;
- the Debug firmware containing the ADC diagnostics and JSON telemetry has been
  flashed and physically verified on the NUCLEO through COM3.

The next step is adding a host-side telemetry parser and tests that accept the
schema-1 record while rejecting malformed JSON, missing fields, wrong types, and
unsupported schema versions. Bidirectional UART commands and the local
serial-to-MQTT gateway follow that validation boundary.

## Learning Strategy

Do not start with a six-month curriculum.

Use the project itself to teach:
- GPIO
- sensor buses
- interrupts
- timing
- firmware structure
- debugging
- RTOS concepts
- build systems
- firmware deployment

The user wants exact, actionable instructions and expects the lead embedded engineer to make most architecture and implementation decisions.

## Career Value

The final system should be presentable for roles such as:
- Embedded Software Engineer
- Firmware Engineer
- Embedded DevOps Engineer
- Firmware CI/CD Engineer
- Build & Release Engineer
- IoT Platform Engineer
- DevSecOps Engineer for connected devices
- Embedded Linux / platform-adjacent engineering
- automotive / industrial / medical-device infrastructure roles

## Portfolio Narrative

The strongest story is that the user's EE background and current DevOps experience converge in this project.

A future resume description could resemble:

> Built an STM32-based embedded telemetry platform using C, sensor interfaces, real-time firmware concepts, automated GitHub Actions firmware builds, static analysis, versioned firmware artifacts, and hardware-in-the-loop testing. Integrated device telemetry with a containerized MQTT/API/PostgreSQL/Grafana stack deployed to Hetzner and added secure firmware release practices.

This text should be updated only after those capabilities actually exist.
