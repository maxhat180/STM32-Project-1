#include <Arduino_BMI270_BMM150.h>
bool imu_ready = false; 
int test_read = 0; 
uint time_now = millis(); 
void setup() {
  Serial.begin(115200);
  imu_ready = IMU.begin();
  if (imu_ready)
  {
    Serial.println("IMU ready");
    test_read = IMU.accelerationAvailable();
    Serial.println("Test reading: "); 
    Serial.println(test_read); 
  }
  else
  {
    Serial.println("IMU Initialization failed.");
  }
  float x,y,z;
}

void loop() {
  if(millis() - time_now >= 300)
  {
    float x,y,z; 
    if (imu_ready && IMU.accelerationAvailable())
    {
      if(IMU.readAcceleration(x, y, z))
      {
        Serial.print("x: ");
        Serial.print(x);
        Serial.print(" y: ,");
        Serial.print(y);
        Serial.print(" z: ");
        Serial.println(z);
        time_now = millis(); 
      } 
    }
  }
}
