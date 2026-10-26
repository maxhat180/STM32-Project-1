# 02 - Onboard accelerometer

Status: initialization and rate-limited X/Y/Z readings working on hardware per
developer report. Source reviewed October 26, 2026; remaining work below.

## Concept

The accelerometer is a separate chip that measures specific force along three
axes. The MCU configures it and retrieves measurements over the onboard I2C bus.
The library handles bus transactions and register configuration; we will inspect
those mechanisms as we progress. No external connections are required.

## First task: initialize the sensor

1. Install **Arduino_BMI270_BMM150** by Arduino in Library Manager. Record the
   installed version below.
2. Create a sketch here named `02_accelerometer.ino`, keeping `01_blink` intact.
3. Include `<Arduino_BMI270_BMM150.h>` and initialize serial at 115200.
4. Call `IMU.begin()` once in `setup()`. It returns success/failure. Print
   `IMU ready` on success or `IMU initialization failed` on failure. Leave `loop()`
   empty for this first checkpoint; do not attempt reads on failure.
5. Upload, open Serial Monitor, then press the board's reset button with the
   monitor open to capture the startup message. If no message appears, report
   that observation rather than assuming initialization failed.

The developer completed this checkpoint and added acceleration reads and
non-blocking rate control in `02_accelerometer.ino`.

## Verification notes

- Board package/version: pending
- Library version: pending
- COM port: pending
- Compile/upload: developer reports successful compile, flash, and live readings.
- Source reviewed: `IMU.begin()` checked, new-data check, successful-read check,
  X/Y/Z serial output, and timestamp update only after a successful read.
- Saved interval is 300 ms (about 3.33 successful reads/second at best), although
  the original task suggested 500 ms. Actual timing was not measured.
- Hardware observations: developer identified X/Y changes with tilt and understood
  Z via face-up/face-down orientation. No numeric measurements were captured.
- No assistant-run build or independent hardware test was performed.

## Bugs and lessons

The first line was initially `include <Arduino_BMI270_BMM150.h> as IMU;`.
The missing `#` caused a compilation error; `as IMU` is not C++ syntax. The header
already declares the `IMU` object. Fixing the include resolved the reported error.
The include is now correct with no trailing semicolon. Other lessons: semicolons
after function calls, separate `Serial.print()` calls instead of printf-style
arguments to `println()`, local-variable scope, and checking read success before
using output parameters.

## Datasheet and measurement concepts covered

- BMI270 combines a three-axis accelerometer and three-axis gyroscope; BMM150 is
  a three-axis magnetometer. Neither directly measures travel speed.
- Degrees per second (dps): 90 degrees/second for 2 seconds gives 180 degrees.
- At +/-4 g, sensitivity is 8192 LSB/g: a raw value of 8192 represents 1 g,
  approximately 9.81 m/s^2. The Arduino API already returns g, so do not divide
  its float output by 8192 again.
- Larger measurement range trades away resolution for the same output bit depth;
  resolution is different from accuracy.
- Accelerometers measure specific force. Stationary support against gravity
  produces approximately 1 g in total, projected across axes. Tilt can be inferred
  under static conditions; motion acceleration complicates that inference.
- Application read/report rate is separate from sensor output data rate.

Official datasheets used for practice:
- [BMI270](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf)
- [BMM150](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmm150-ds001.pdf)

## Resume here

Keep the developer as the primary author. Do not repeat completed read/print work.

1. Review timing: replace global `uint time_now = millis()` with an
   `unsigned long` initialized to zero; assign `millis()` in `setup()`. Use a named
   interval and agree whether 300 or 500 ms is desired. Remove unused setup-local
   `x/y/z` and clarify/remove the one-time readiness diagnostic (`test_read`).
2. Record library/core versions and COM port, then capture a six-face static test:
   each axis pointing up and down. Expect roughly +/-1 g on that axis and near
   zero on the other two; log real observations without claiming calibration.
3. Have the developer compute sqrt(x*x + y*y + z*z), expecting approximately 1 g
   while stationary. Discuss deviations, noise, and why a slow application read
   rate can miss short motion events.
4. Read the datasheet output-rate/range settings and compare with the actual
   installed library implementation. Distinguish raw counts, g, ODR, and reporting.
5. Close this section with documented evidence, then start motion detection with
   thresholds, filtering, and hysteresis. Static tilt estimation is an optional
   extension; gyroscope and magnetometer work are later tasks.

## References

- [Rev2 library selection](https://support.arduino.cc/hc/en-us/articles/11729186296476-Use-the-new-sensor-libraries-for-Nano-33-BLE-Rev2-and-Nano-BLE-Sense-Rev2)
- [Library and examples](https://github.com/arduino-libraries/Arduino_BMI270_BMM150)
