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

## Available Hardware Known So Far

Confirmed:
- STMicroelectronics NUCLEO-F446RE
- Arduino board
- Arduino starter kit
- multiple sensors/modules
- jumper wires
- 2 breadboards

The exact sensor/module inventory has NOT yet been captured.

See `HARDWARE_INVENTORY.md`.

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
