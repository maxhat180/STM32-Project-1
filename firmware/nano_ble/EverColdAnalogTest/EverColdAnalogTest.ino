#include <ArduinoBLE.h>  // Make ArduinoBLE classes and functions available.

BLEService sensorService(  // Create a global BLE service that lives for the entire program.
  "7e400001-b5a3-f393-e0a9-e50e24dcca9e"  // Custom UUID identifying our sensor service.
);

BLEFloatCharacteristic temperatureCharacteristic(  // Create a characteristic holding a four-byte float in Celsius.
  "7e400002-b5a3-f393-e0a9-e50e24dcca9e",  // UUID matched by the Python receiver.
  BLERead | BLENotify  // Combine flags to allow reads and subscribed updates.
);

void setup() {  // Arduino calls this once after startup or reset.
  Serial.begin(115200);  // Initialize USB serial with the selected baud setting.
  analogReadResolution(12);  // Return 12-bit ADC counts, from 0 to 4095.

  // Wait until the USB Serial Monitor connects.
  while (!Serial) {  // Wait for the laptop to open Serial Monitor before continuing.
    delay(10);  // Pause for 10 ms between serial connection checks.
  }

  Serial.println("Sensor test starting");  // Print a startup message followed by a newline.
  if (!BLE.begin()) {  // Initialize BLE; enter this block if initialization fails.
    Serial.println("BLE initialization failed");  // Report the startup failure over USB.
    while (true) {  // Stay here permanently because BLE could not start.
      delay(100);  // Pause between iterations of the failure loop.
    }
  }

  BLE.setLocalName("EverCold");  // Set the name reported during BLE discovery.
  BLE.setAdvertisedService(sensorService);  // Include our service UUID in advertising.

  sensorService.addCharacteristic(temperatureCharacteristic);  // Attach the temperature field to our service.
  BLE.addService(sensorService);  // Register the service with the BLE stack.

  BLE.advertise();  // Start advertising so the laptop can discover and connect.
  Serial.println("EverCold BLE advertising");  // Report that advertising has started.
}

void loop() {  // Arduino calls this repeatedly after setup finishes.
  BLE.poll();  // Service BLE events on every pass through the loop.

  static unsigned long lastSampleMs = 0;  // Keep the last sample time between loop calls; initialize once.
  unsigned long nowMs = millis();  // Read elapsed milliseconds since startup.

  if (nowMs - lastSampleMs < 2000) {  // Wait two seconds between samples; unsigned subtraction handles timer wrap.
    return;  // End this loop call early; Arduino immediately calls loop again.
  }
  lastSampleMs = nowMs;  // Record the time of this sample.

  int temperatureRaw = analogRead(A0);  // Measure the TMP36 output as an ADC count.
  int lightRaw = analogRead(A1);  // Measure the light-divider junction as an ADC count.

  float temperatureVoltage = temperatureRaw * 3.3f / 4095.0f;  // Estimate volts using a nominal 3.3 V range; f marks float constants.
  float temperatureC = (temperatureVoltage - 0.5f) * 100.0f;  // Apply TMP36 offset (0.5 V) and slope (0.01 V per degree C).

  temperatureCharacteristic.writeValue(temperatureC);  // Update the BLE value and notify a subscribed laptop.

  Serial.print("Temperature C: ");  // Print a label without ending the line.
  Serial.print(temperatureC, 1);  // Print temperature with one decimal place.
  Serial.print(" | Light: ");  // Print a separator and light label.
  Serial.print(lightRaw);  // Print the raw light ADC count.
  Serial.print(" | BLE connected: ");  // Print a connection-status label.
  Serial.println(BLE.connected() ? "yes" : "no");  // Choose yes/no from connection state and end the line.
}
