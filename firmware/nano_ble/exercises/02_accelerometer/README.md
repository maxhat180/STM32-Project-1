# 02 - Onboard accelerometer

Status: assigned, not yet implemented or hardware-verified.

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

Write the sketch yourself; no complete solution is provided. Acceleration reads
and non-blocking reporting will be the next task after this checkpoint.

## Verification notes

- Board package/version: pending
- Library version: pending
- COM port: pending
- Compile/upload: not yet checked
- Startup message: not yet checked
- Debugging notes: none yet

## References

- [Rev2 library selection](https://support.arduino.cc/hc/en-us/articles/11729186296476-Use-the-new-sensor-libraries-for-Nano-33-BLE-Rev2-and-Nano-BLE-Sense-Rev2)
- [Library and examples](https://github.com/arduino-libraries/Arduino_BMI270_BMM150)
