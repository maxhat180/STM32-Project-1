# Project Ideas and Roadmap

These ideas were discussed before moving the project into Codex.

The intention is NOT to create six unrelated repositories.

Treat them as phases of one evolving embedded DevOps platform.

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

Connect the physical embedded device to the user's Hetzner server.

Potential architecture:

STM32
  |
  v
Serial / gateway
  |
  v
MQTT
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

# Recommended Compressed Sequence

Because time is limited:

## Milestone 1
STM32 + 2-3 sensors + structured serial telemetry

## Milestone 2
GitHub Actions automated firmware build/test/artifacts

## Milestone 3
Telemetry path to Hetzner

## Milestone 4
PostgreSQL + dashboard

## Milestone 5
Hardware-in-the-loop CI

## Milestone 6
RTOS / Zephyr refactor

## Milestone 7
Secure firmware releases

Only after those are working should CAN / multi-controller work be considered.
