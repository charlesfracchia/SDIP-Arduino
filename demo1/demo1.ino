// Arduino as OBMP data source without access to time-synchronization or a real-time clock
// See http://playground.arduino.cc/Code/time for examples of synchronizing time

// For this version, lacking an absolute time reference, 
// the computer should record the time it receives the first 
// serial character from this device as the effective start time.

#include <Streaming.h>

unsigned long time;
unsigned long last_time;
unsigned int num_samples;
int i;
String preamble;
String sensor_name;
String address;

void setup() {
  
  // Customize your board's "address" and the sensor name here:
  address = "my arduino's name";
  sensor_name = "A0";
  
  preamble = "{\"sender\":{\"address\":\"" + address + "\",\"time-format\":\"microsecs\",\"start-time\":[]},\"data\":[{\"name\":";

  num_samples = 100;
  i = 0;
  
  Serial.begin(115200);

  last_time = micros();
}
void loop() {
  if (i == 0){
      Serial << preamble;
      Serial << "\"" << sensor_name << "\"";
      Serial << ",\"vals\":[";  
      i++;
  
  } else if (i > 0 && i < num_samples){
      time = micros() - last_time;
      Serial << "{\"time\":" << time << ",\"val\":" << analogRead(A0) << "},\n";
      i++;
    
  } else if (i == num_samples){
       Serial << "{\"time\":" << time << ",\"val\":" << analogRead(A0) << "}]}]}\n\n";
       i = 0;
       last_time = micros();
  }
}
