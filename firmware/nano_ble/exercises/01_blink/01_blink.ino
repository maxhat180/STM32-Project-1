unsigned long time_now = 0;
char serial_in = {}; 
bool blinking_enabled = true;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 
  time_now = millis();
  Serial.begin(115200);
}

bool led_status = true; 
unsigned long time_elapsed = 0;
// the loop function runs over and over again forever
void loop() {
  if (Serial.available() > 0)
  {
    serial_in = Serial.read(); 
    if(serial_in == 's')
    {
      Serial.println("Blinking is disabled");
      blinking_enabled = false;   
    }
    else if (serial_in == 'r')
    {
      Serial.println("Blinking is enabled"); 
      blinking_enabled = true; 
      time_now = millis(); 
    }
  }
  if (blinking_enabled == true)
  { 
        time_elapsed = millis() - time_now; 
        if (time_elapsed >= 500)
        {
          if (led_status == true)
          {
            digitalWrite(LED_BUILTIN, LOW);       
            led_status = false; 
            time_now = millis();
          }
        else
        {
          digitalWrite(LED_BUILTIN, HIGH);       
          led_status = true; 
          time_now = millis();
        }
        }
    }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    led_status = false;
  }
}
