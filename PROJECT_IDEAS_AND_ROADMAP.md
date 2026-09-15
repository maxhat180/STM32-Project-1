# Project Ideas and Roadmap

These ideas were discussed before moving the project into Codex.

The intention is NOT to create six unrelated repositories.

Treat them as phases of one evolving embedded DevOps platform.

## Progress Snapshot - September 15, 2026

- Phase 1 is partially complete: GPIO, UART, TMP36 temperature, and photoresistor
  light telemetry are working on the physical NUCLEO-F446RE.
- Phase 2 has an operational baseline: GitHub Actions cross-compiles Release
  firmware and publishes timestamped, checksummed ELF/BIN/HEX deployment packages.
  The one-command Windows flashing path has been tested on the real board.
- Host-side C unit tests now cover the pure ADC/TMP36 conversion module and run as
  a required GitHub Actions gate before firmware packaging.
- Static analysis, explicit size gates, tagged releases, and signing remain future
  Phase 2 increments.
- Hardware-in-the-loop CI, gateway, server, observability, and advanced firmware
  phases have not started.

The immediate next increment is completing explicit ADC start, poll, and stop
error handling. Start failure is now detected and reported; rank-specific poll
diagnostics and stop-result handling remain.

The longer path must also include deeper UART work, I2C and SPI sensor drivers,
additional physical sensors, a laptop-hosted MQTT gateway/broker milestone, and an
eventual compact custom temperature-telemetry PCB. RF-component availability and
Israeli import/customs requirements are explicit design and procurement
constraints for the Bluetooth/LTE phase.

---

# 1. STM32 Environmental / Sensor Monitoring Device

## Goal

Use the NUCLEO-F446RE with several sensors from the user's existing starter kit.

Potential inputs:
- temperature
- humidity
- light
- potentiometer
- motion
- distance
- buttons
- other available sensors

## Embedded Skills

- C
- GPIO
- ADC
- UART
- UART receive, framing, parsing, commands, timeouts, and error recovery
- I2C
- SPI
- timers
- interrupts
- sensor drivers
- debouncing
- structured firmware
- ST-LINK debugging

## Desired Telemetry Shape

Example:

```json
{
  "device": "stm32-f446-001",
  "temperature": 23.4,
  "humidity": 47.2,
  "light": 612,
  "uptime": 48192,
  "firmware": "1.0.0"
}
```

This phase should establish a working embedded device quickly.

---

# 2. Embedded CI/CD Pipeline

## Goal

Every PR / push should automatically verify firmware quality.

Possible pipeline:

GitHub Push / PR
    |
    v
Install ARM toolchain
    |
    v
Compile firmware
    |
    v
Run unit tests
    |
    v
Static analysis
    |
    v
Firmware size checks
    |
    v
Generate `.bin` / `.hex`
    |
    v
Upload versioned artifacts

## Skills

- GitHub Actions
- cross compilation
- firmware artifacts
- test automation
- static analysis
- release engineering

This should start early rather than being postponed until the end.

---

# 3. Hardware-in-the-Loop CI

## Goal

Make CI test a real physical NUCLEO board.

Architecture:

GitHub Actions
    |
    v
Self-hosted runner
    |
    v
ST-LINK
    |
    v
Flash NUCLEO
    |
    v
Firmware boots
    |
    v
Python test harness reads UART
    |
    v
PASS / FAIL

Example boot-test expectations:

```text
BOOT_OK
SELF_TEST_GPIO_OK
SENSOR_INIT_OK
FW_VERSION=1.2.0
```

Later tests could verify real electrical behavior.

Example:
- firmware drives one GPIO high
- another test circuit or second MCU verifies the signal
- CI marks the hardware test pass/fail

This phase is particularly valuable for embedded DevOps / firmware infrastructure roles.

---

# 4. RTOS Sensor Hub

## Goal

Refactor the firmware into a real-time design.

Preferred later RTOS:
- Zephyr

FreeRTOS is also acceptable.

Conceptual architecture:

Temperature Task ----\
Humidity Task --------+--> Queue --> Processing --> Telemetry
Light Task -----------/

## Skills

- tasks/threads
- queues
- semaphores
- mutexes
- scheduling
- race conditions
- watchdogs
- fault handling
- device tree concepts if using Zephyr

This should happen after the initial bare-metal/HAL system is working.

---

# 5. IoT Telemetry Platform

## Goal

First connect the physical embedded device to a local MQTT system on the user's
laptop. After that path works, move the broker and downstream services to the
user's Hetzner server.

Potential architecture:

STM32
  |
  v
USB serial / Python gateway
   |
   v
Mosquitto MQTT (laptop first, Hetzner later)
  |
  v
Backend ingestion
  |
  v
PostgreSQL
  |
  v
Grafana / dashboard

Possible monitored values:
- device online/offline
- firmware version
- temperature
- humidity
- light
- supply voltage
- uptime
- reboot count
- fault count
- sensor status
- last telemetry timestamp

## DevOps Skills

- Docker
- Docker Compose
- MQTT
- PostgreSQL
- backend APIs
- observability
- deployment
- TLS
- service health
- log management

The gateway should initially add trustworthy wall-clock timestamps because the
MCU does not yet have a persistent synchronized time source. A later firmware/PCB
phase can introduce an RTC, backup power, or network-assisted time synchronization.

---

# 6. Secure Firmware Release / Update System

## Goal

Build DevSecOps practices into firmware delivery.

Example release pipeline:

Git tag
   |
   v
GitHub Actions
   |
   +--> build
   +--> test
   +--> static analysis
   +--> create firmware.bin
   +--> checksum
   +--> sign artifact
   |
   v
Versioned release

Future device/gateway logic:

Firmware downloaded
   |
   v
Verify signature
   |
   +--> valid --> install
   |
   +--> invalid --> reject

## Skills

- bootloader concepts
- flash layout
- firmware signing
- cryptographic integrity
- rollback
- versioning
- secure releases
- SBOM/dependency tracking

Over-the-air update capability is optional and should only be added when the underlying design supports it safely.

---

# 7. Mini Industrial Controller

## Goal

Evolve the system toward industrial/automotive-style embedded design.

Potential future hardware:
- second STM32 board
- CAN transceivers

Architecture:

Controller A
    |
    | CAN
    |
Controller B

Possible messages:
- RPM
- temperature
- current
- state
- faults
- heartbeat
- commands

Fault scenarios:
- missing heartbeat
- sensor failure
- invalid packet
- device reboot
- timeout

## Skills

- CAN
- state machines
- fault handling
- watchdogs
- communications protocols
- robust embedded architecture

This is an advanced optional extension.

---

# 8. Compact Custom Temperature-Telemetry PCB

## Goal

Turn the proven development-board system into a compact custom PCB containing a
microcontroller, temperature sensor, power circuitry, debug/programming access,
and an optional Bluetooth or LTE transport. The device should send timestamped
temperature telemetry to the project's MQTT or HTTPS ingestion path.

Develop it incrementally:

1. freeze tested sensor, telemetry, and transport interfaces in firmware;
2. define power, timestamp accuracy, reporting interval, enclosure, and size
   requirements;
3. build a non-RF first revision or use a connectorized radio module;
4. perform schematic review, PCB layout, design-rule checks, and bring-up planning;
5. source RF modules locally or through a viable approved route;
6. verify current Israeli regulatory/import requirements before committing to a
   Bluetooth/LTE module and antenna design;
7. integrate RF only after the core PCB and wired telemetry path are proven.

Keep the transport replaceable so RF sourcing cannot block the rest of the system:

```text
Sensors -> application telemetry -> UART / Bluetooth / LTE adapter
```

Important skills include schematic capture, component selection, power budgeting,
PCB layout, decoupling, ESD/protection, SWD access, antenna keep-outs, EMC-aware
design, board bring-up, and design-for-test.

---

# Recommended Compressed Sequence

Because time is limited:

## Milestone 1
STM32 GPIO, UART transmit, TMP36, and photoresistor bring-up (complete)

## Milestone 2
GitHub Actions build and flashable timestamped artifacts (baseline complete)

## Milestone 3
Pure firmware modules, host unit tests, ADC hardening, and structured telemetry

## Milestone 4
Bidirectional UART plus Python gateway and Mosquitto MQTT on the laptop

## Milestone 5
I2C, SPI, and additional physical sensor drivers

## Milestone 6
Move MQTT/ingestion to Hetzner; add PostgreSQL and a dashboard

## Milestone 7
Hardware-in-the-loop CI, watchdog/fault work, and RTOS/Zephyr increment

## Milestone 8
Secure firmware releases and compact custom temperature-telemetry PCB

Only after those are working should CAN / multi-controller work be considered.
