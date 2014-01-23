#include <SoftwareSerial.h>
//include <Streaming.h>

SoftwareSerial mySerial(1,2);

// These constants won't change.  They're used to give names
// to the pins used:
unsigned long time;
unsigned long last_time;
unsigned int num_samples;
int i = 0;
String sensorName = "Light Sensor";
String address = "Charles' Bench";
String preamble = "{\"sender\":{\"address\":\"" + address + "\",\"time-format\":\"microsecs\",\"start-time\":[]},\"data\":[{\"name\":";
String line;

const int analogInPin = 3;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value read from the pot

void setup() {
  // initialize serial communications at 9600 bps:
  mySerial.begin(9600);
  last_time = micros();
}

void loop() {
  if (i == 0){
    line + preamble + "\"" + sensorName + "\"" + ",\"vals\":[";
    i++;
  
  } else if (i > 0 && i < num_samples){
    time = micros() - last_time;
    sensorValue = analogRead(analogInPin);
    line + "{\"time\":" + time + ",\"val\":" + map(sensorValue, 0, 1023, 0, 255) + "},\n";
    i++;
    
  } else if (i == num_samples){
    sensorValue = analogRead(analogInPin);
    line + "{\"time\":" + time + ",\"val\":" + map(sensorValue, 0, 1023, 0, 255) + "}]}]}\n\n";
    i = 0;
    last_time = micros();
    mySerial.print(line);
  }                     
}
