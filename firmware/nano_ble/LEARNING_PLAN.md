# Nano 33 BLE Sense Rev2 learning plan

Last updated: October 26, 2026.

## Direction and teaching approach

Build a sensor node that can join the existing telemetry system. The developer
writes the firmware in small steps; explain interfaces, review code, and debug
from observations. Use the onboard hardware and USB cable first. Skip repetitive
exercises once understanding is demonstrated. Inspect library internals and
introduce lower-level work as it becomes useful.

## Progress and milestones

| Milestone | Work | Status |
| --- | --- | --- |
| 1. Board fundamentals | Manual upload, GPIO, blink patterns, `millis()`, serial input, stop/resume | Developer reports working; source reviewed |
| 2. Acceleration acquisition | Initialize BMI270 through the official library; read X/Y/Z; understand g, gravity, axes, I2C, and sampling versus reporting rate | Next; initialization task assigned |
| 3. Motion detection | Thresholds, filtering, hysteresis, and experiments with stationary/moving data | Planned |
| 4. Firmware structure and reliability | Separate acquisition, detection, and reporting; handle initialization failure, stale data, and missed samples | Planned |
| 5. BLE telemetry | Services, characteristics, notifications, and a Python receiver; reuse the earlier prototype where useful | Planned; earlier EverCold analog/BLE prototype exists separately |
| 6. Engineering and integration | Host tests for detection logic, Nano firmware CI, then gateway/MQTT/server telemetry integration | Planned; existing CI currently targets STM32 |

The optional serial speed-selection exercise was skipped by agreement. The
first exercise evolved in place into non-blocking blink with serial stop/resume.
See [exercise notes](exercises/01_blink/README.md) for evidence and debugging.

## Verified next hardware interface

The Rev2's BMI270 provides acceleration and angular velocity; the BMM150 provides
magnetic-field measurements. The onboard IMU connects to the MCU over I2C. Use
`Arduino_BMI270_BMM150`, not the original Sense board's `Arduino_LSM9DS1` library.
No external wiring is needed.

Start with [02_accelerometer](exercises/02_accelerometer/README.md). After successful
initialization, introduce `IMU.accelerationAvailable()` and
`IMU.readAcceleration(x, y, z)` with `float` variables. Acceleration is reported in
g; a stationary supported board should have a vector magnitude near 1 g, rather
than zero on every axis. Later distinguish sensor output rate from serial print
rate and handle failed reads before using data.

Sources checked October 26, 2026:

- [Arduino Rev2 sensor library guidance](https://support.arduino.cc/hc/en-us/articles/11729186296476-Use-the-new-sensor-libraries-for-Nano-33-BLE-Rev2-and-Nano-BLE-Sense-Rev2)
- [Official IMU library documentation](https://github.com/arduino-libraries/Arduino_BMI270_BMM150/blob/master/docs/readme.md)
- [Official accelerometer example](https://github.com/arduino-libraries/Arduino_BMI270_BMM150/blob/master/examples/SimpleAccelerometer/SimpleAccelerometer.ino)

## Later option: wireless firmware updates

The normal Arduino setup uploads over USB. BLE firmware updates require a
compatible update implementation and bootloader; adding ArduinoBLE alone does
not enable wireless sketch uploads. This is a later bootloader/release-security
project, not a change needed for the sensor exercise. Establish signing,
validation, and recovery before adopting it. No bootloader changes have been made.

- [Board datasheet: USB bootloader](https://docs-content.arduino.cc/resources/datasheets/ABX00069-datasheet.pdf)
- [Nordic DFU app and compatible bootloaders](https://www.nordicsemi.com/Products/Development-tools/nRF-Device-Firmware-Update)
