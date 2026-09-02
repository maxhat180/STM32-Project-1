# Hardware Inventory

## Status

Partially verified from six packaging photos supplied on August 31, 2026 and eight
opened-hardware photos supplied on September 1, 2026.

The NUCLEO board and the loose SparkFun sensor-kit parts are now visible. The Seeed
Sidekick kit contents are still recorded from the manufacturer's contents list rather
than from an overhead photo of its loose parts.

## Development Boards

| Item | Exact model / label | Quantity | Evidence | Notes |
|---|---|---:|---|---|
| Primary STM32 board | STMicroelectronics NUCLEO-F446RE, MB1136 rev C | 1 | Front/back board photos and rear label `NUCLEO-F446RE`, `NUF446RE$KU1` | Visually verified. Integrated ST-LINK programmer/debugger and Virtual COM Port; Arduino and ST Morpho headers fitted. |
| Secondary board | Arduino Uno Rev3, model `A000066` | 1 | Box/compliance card labeled `ARDUINO UNO Rev3` | Confirmed from packaging; board itself was not needed for Milestone 1. |

### NUCLEO Bring-up Verification

Verified over USB on September 1, 2026 using STM32CubeProgrammer 2.20.0:

- Board detected as `NUCLEO-F446RE`.
- ST-LINK serial number: `066DFF515086874967255548`.
- ST-LINK firmware: `V2J46M33`.
- Virtual COM Port: `COM3`.
- SWD connection: successful at 4000 kHz under hardware reset.
- Target voltage: 3.24 V.
- Target identified as STM32F446xx, device ID `0x421`, revision A.
- Flash size: 512 KBytes.
- CPU: Arm Cortex-M4.

No firmware was erased or programmed during this verification.

## Sensor Kit

### SparkFun Essential Sensor Kit V2

- SparkFun SKU: `SEN-20408`
- DigiKey label: `1568-SEN-20408-ND`
- Quantity: 1 kit
- Evidence: sealed/labeled SparkFun package
- Manufacturer page: <https://www.sparkfun.com/sparkfun-essential-sensor-kit-v2.html>

Manufacturer-listed contents:

| Item | Part / value | Expected quantity | Interface / use | Verification status |
|---|---|---:|---|---|
| Triple-axis accelerometer breakout | SparkFun SEN-13963, LIS3DH | 1 | Digital motion/orientation sensor supporting I2C/SPI | Visually verified in labeled bag; breakout headers are not soldered |
| Hall-effect sensor | TO-92 package; exact marking pending | 1 | Digital magnetic-field/presence input | Visually present; identity/pinout not yet safe to infer |
| Tilt sensor | AT407-style metal can | 1 | Digital switch input | Visually present |
| Piezo elements | Wired discs | 2 | Analog vibration/knock sensing | Visually verified |
| Insulated reed switch | Marking `59165-1-U-00-A 2224G` | 1 | Digital magnetic switch | Visually verified |
| IR receiver diode/module | TSOP382 family; suffix pending | 1 | Infrared remote receiver | Expected; not visually verified |
| Infrared LED | 940-950 nm listed | 1 | Infrared transmitter | Expected; not visually verified |
| Temperature sensor | TMP36, TO-92 package | 1 | Analog voltage output | Visually present; final pin orientation must be confirmed from the marking/datasheet before wiring |
| Trimmer potentiometer | 10 kOhm, blue top-adjust, marked `W103` | 1 | Analog input | Visually verified |
| Square magnet | 0.25 inch | 1 | Used with reed/Hall sensors | Expected; not visually verified |
| Force-sensitive resistor | Round Interlink/FSR-family sensor, 0.5 inch sensing area listed | 1 | Variable resistance / analog input | Visually verified; exact ordering suffix not visible |
| Mini photocell | Approx. 1 kOhm light / 10 kOhm dark per kit page | 1 | Variable resistance / analog light sensing | Visually verified |
| Flex sensor | Spectra Symbol, marking `007 22 38` | 1 | Variable resistance / analog bend sensing | Visually verified |
| Piezo buzzer/speaker | 12 mm, approximately 2 kHz | 1 | Audio output | Visually verified |
| Resistors | 1 MOhm, 1/6 W, +/-5% | 2 | Bias/load components | Expected; not visually verified |

## Component / Prototyping Kit

### Seeed Studio Sidekick Basic Kit for Arduino V2

- Label/product number: `110060025`
- Quantity: 1 kit
- Evidence: labeled green Seeed Studio case
- Manufacturer documentation: <https://wiki.seeedstudio.com/Sidekick_Basic_Kit_for_Arduino_V2/>

Manufacturer-listed contents:

| Item | Value/type | Expected quantity | Verification status |
|---|---|---:|---|
| Solderless breadboard | 2 x 30 terminal columns plus power rails | 1 | Expected; case not opened in photos |
| Green LEDs | Discrete | 5 | Expected; not visually verified |
| Red LEDs | Discrete | 5 | Expected; not visually verified |
| RGB LED | Common anode | 1 | Expected; not visually verified |
| Ceramic capacitors | 10 nF | 10 | Expected; not visually verified |
| Ceramic capacitors | 100 nF | 10 | Expected; not visually verified |
| Aluminum electrolytic capacitors | 100 uF | 5 | Expected; polarity must be observed |
| Resistors | 330 Ohm | 10 | Expected; not visually verified |
| Resistors | 1 kOhm | 10 | Expected; not visually verified |
| Resistors | 10 kOhm | 10 | Expected; not visually verified |
| Tilt switch | Exact part pending | 1 | Expected; not visually verified |
| Thermistor | Exact resistance/curve pending | 1 | Expected; not visually verified |
| Photoresistor | Exact model pending | 1 | Expected; not visually verified |
| Diode | Exact model/polarity marking pending | 1 | Expected; not visually verified |
| Buzzer | Exact active/passive type pending | 1 | Expected; not visually verified |
| Push button | Momentary | 1 | Expected; not visually verified |
| Slide switches | Exact style pending | 5 | Expected; not visually verified |
| Mini servo | Exact model/current rating pending | 1 | Expected; power separately or only after current requirements are verified |
| Potentiometer with knob | Exact resistance pending | 1 | Expected; not visually verified |
| Breadboard jumper wires | 5 long, 20 short | 25 | Expected; not visually verified |
| Internal organizer boxes | Kit listing says 4 | 4 | Expected; not visually verified |

## Cables and Additional Jumper Wires

| Item | Quantity | Evidence | Notes |
|---|---:|---|---|
| USB 2.0 Type-A to Mini-B cable, approximately 1.5 m | 1 | Purchase receipt and cable photo | Intended for the NUCLEO ST-LINK USB connector; connector should be checked against the physical board before insertion. |
| Colored jumper-wire bundle, male-to-male | 60 listed on receipt | Receipt plus photo | Receipt appears to list 5 cm; exact length/count can be checked when unpacked. |
| Colored jumper-wire bundle, male-to-female | 60 listed on receipt | Receipt plus photo | Receipt appears to list 2.5 cm; exact length/count can be checked when unpacked. |
| Second USB cable visible in jumper-wire package | 1 visible | Photo | Connector types are not clear enough to identify safely. |

## Other Previously Reported Hardware

| Item | Quantity | Status |
|---|---:|---|
| Additional breadboard | 1 | User previously reported two breadboards total; only the one expected in the Seeed kit is currently documented by product contents. |

## Recommended Milestone 1 Candidates

The selected low-risk Milestone 1 combination is:

1. **TMP36 temperature sensor** - introduces a calibrated ADC measurement.
2. **Photoresistor plus a 10 kOhm resistor** - provides a second ADC channel and an easy light/dark test stimulus.
3. **The NUCLEO's onboard B1 user button and LD2 user LED** - introduces GPIO input, debouncing/state handling, and GPIO output without extra wiring.

This combination is intentionally low risk and gives a useful telemetry demo without
requiring a motor supply. The verified LIS3DH accelerometer is a strong Milestone 2
candidate for adding I2C, interrupts, and motion telemetry, but its breakout currently
needs header pins soldered before it can be wired reliably.

## Photos Still Needed Before Wiring

Open the packages and provide:

1. One overhead photo of the opened Seeed Sidekick kit, primarily to verify the breadboard, 10 kOhm resistors, and jumper wires needed for Milestone 1.
2. A sharper close-up of the flat marked face of the TMP36 before finalizing its left/center/right pin wiring.
3. A photo of both ends of the Mini-B USB cable before its first connection.
4. Later, close-ups of the IR receiver, Hall sensor, thermistor, diode, buzzer, servo, and both sides of the LIS3DH breakout when those parts enter scope.

## Safety Rules

Before providing wiring instructions:

- identify the exact physical component and pin orientation;
- determine its operating voltage and output voltage;
- confirm that every signal presented to the STM32 is 3.3 V safe;
- confirm the NUCLEO-F446RE Arduino-header and STM32 GPIO mapping from the current ST user manual;
- state exact board connector labels and MCU pin names;
- do not power the servo from the STM32 3.3 V rail;
- do not apply 5 V logic to an STM32 GPIO unless that exact pin's tolerance and configuration have been verified.

Do not infer a pinout solely from a component's general appearance.
