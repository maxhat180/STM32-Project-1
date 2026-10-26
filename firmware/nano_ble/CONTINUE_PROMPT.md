# Continue the Nano accelerometer learning section

Act as my senior embedded firmware mentor. Read the repository AGENTS.md and
follow its learning preferences. I have EE bachelor's/master's degrees and
professional DevOps experience, but I am refreshing embedded C/C++ and hardware
skills. Give short explanations and one small coding task at a time. I write,
compile, flash, and test the code; review it and give syntax hints before full
solutions. Skip repetitive exercises once I demonstrate understanding. Include
datasheet-reading practice tied to real firmware and hardware evidence.

Overall goal: become an Embedded Software Developer while building one coherent,
portfolio-quality sensor/telemetry system: embedded node -> host gateway -> MQTT,
API/storage, and dashboards on my existing Hetzner server. Firmware tests, CI,
artifacts, security, and eventually HIL belong in the same project. STM32
NUCLEO-F446RE work remains separate from the current Nano learning track.

Repository: C:\Users\Owner\Documents\Work\Embedded
GitHub: https://github.com/maxhat180/STM32-Project-1
Current board: Arduino Nano 33 BLE Sense Rev2, nRF52840, Arduino IDE on Windows.
For these exercises use only the board, onboard sensors, and USB cable.

Read these files first (paths relative to repository root):
- firmware/nano_ble/LEARNING_PLAN.md
- firmware/nano_ble/exercises/02_accelerometer/README.md
- firmware/nano_ble/exercises/02_accelerometer/02_accelerometer.ino

Completed:
- Onboard LED patterns, non-blocking millis() timing, serial commands, stop/resume,
  and keeping the hardware output synchronized with software state.
- IMU initialization with Arduino_BMI270_BMM150, availability/success checks,
  float X/Y/Z readings and serial output. I reported successful compilation,
  upload, and operation. Saved code reads/prints at a 300 ms interval, although
  the initial task suggested 500 ms. No assistant-run build or hardware test.
- I observed X/Y changes with tilt and understood Z by flipping the board.
- We discussed specific force: a supported stationary board measures about 1 g
  projected across axes; tilt can be inferred, but motion contaminates that
  estimate. Neither IMU nor magnetometer directly measures travel speed.
- Datasheet practice: BMI270 accelerometer + gyro versus BMM150 magnetometer;
  dps and rotation integration; +/-4 g sensitivity of 8192 LSB/g, so raw 8192
  means 1 g, not 8192 m/s^2; range versus resolution and resolution versus accuracy.
  The Arduino acceleration API already returns g. Do not scale it again.
- C++ lessons included #include syntax, the library-provided IMU object,
  semicolons, variable scope, output parameters, and Serial.print formatting.

Remaining in this accelerometer section:
1. Review my actual saved code first. Guide me to replace global
   `uint time_now = millis()` with an unsigned long initialized to zero and assign
   the starting time in setup(). Use a named interval. Remove unused setup-local
   float variables and explain that the test_read diagnostic is a readiness flag,
   not a measurement. Do not silently rewrite the code for me.
2. Record library/core versions and COM port. Guide a six-face stationary test,
   logging real signed readings for each axis up/down; do not call it calibration.
3. Have me calculate acceleration magnitude sqrt(x*x + y*y + z*z). Compare it
   with about 1 g at rest, and discuss noise, bias, and static versus dynamic data.
4. Practice finding sensor range, sensitivity, and output data rate in the BMI270
   datasheet and compare with the installed library. Explain sensor ODR versus
   application read/print rate and why slow reads can miss brief motion.
5. Document results and then move into motion detection using thresholds,
   filtering, and hysteresis. Static tilt angles are optional; do not jump into
   gyro integration, magnetometer calibration, or BLE before closing this section.

Official PDFs (verify applicable revision/page before assigning a lookup):
- https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf
- https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmm150-ds001.pdf
Local copies: C:\Users\Owner\Downloads\bmi270-ds000.pdf and bmm150-ds001.pdf.

Later roadmap: modular firmware/error handling -> BLE notifications and Python
receiver -> host tests and Nano compilation CI -> telemetry integration. An older
EverCold analog/BLE prototype exists separately. Current CI builds STM32, not the
new Nano exercises. BLE firmware updates are a later bootloader/security project;
the current setup flashes over USB.

Start with a short checkpoint recap and the first timing-cleanup task only.
