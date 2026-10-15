# EverCold Nano BLE prototype

Board: Arduino Nano 33 BLE Sense Rev2. This is a separate target from
`../stm32_telemetry/` and is not built by the existing STM32 CI workflow.

The TMP36 output connects to A0. The light divider is
`3V3 -> photoresistor -> A1 junction -> 10 kOhm -> GND`.
Both circuits share the Nano's 3V3 supply and GND. Inputs must stay within
0-3.3 V. Disconnect USB before changing wiring.

## Manual upload

1. Open `EverColdAnalogTest/EverColdAnalogTest.ino` from this repository in Arduino IDE.
2. Install Arduino Mbed OS Nano Boards and the ArduinoBLE library.
3. Select Arduino Nano 33 BLE and the board's current COM port (COM4 on the verified setup).
4. Upload and open Serial Monitor at 115200.

Keep Serial Monitor open: the sketch waits for USB serial before starting BLE.
It samples both analog inputs every two seconds and exposes temperature as a
four-byte little-endian float in Celsius over BLE. Light is currently printed
only over USB. Temperature uses a nominal 3.3 V ADC range and is not calibrated.

## Laptop receiver

Enable laptop Bluetooth. Run these PowerShell commands from the repository root:

```powershell
py -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r .\gateway\requirements-ble.txt
.\.venv\Scripts\python.exe .\gateway\ble_receiver.py
```

The receiver discovers the advertised name `EverCold`, connects, and subscribes
to temperature notifications. Press Ctrl+C to disconnect. It exits on a dropped
connection; automatic reconnection and MQTT are not implemented yet.

Service UUID: `7e400001-b5a3-f393-e0a9-e50e24dcca9e`.
Temperature characteristic UUID: `7e400002-b5a3-f393-e0a9-e50e24dcca9e`.

## Observed validation

On October 15, 2026, the developer verified manual upload, raw ADC readings,
temperature conversion, both sensors responding to physical changes, and live
temperature notifications in the laptop Python receiver. Relocating these files
does not change the firmware or receiver behavior.
