// Arduino as OBMP data source, synchronizing time with the computer.
// You need to install the Time library from: http://www.pjrc.com/teensy/td_libs_Time.html
// You will also need to run the accompanying Python script to send the sync message over serial to the Arduino

#include <Time.h>
#include <Streaming.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

unsigned long time;
unsigned long last_time;

String preamble; // string representing the beginning of an OBMP JSON packet

// Customize your board's "address" and the sensor name here
String sensor_name = "SENSOR";
String address = "ADDRESS";
unsigned int num_samples = 100; // Number of sensor samples per OBMP JSON packet

void setup() {
    Serial.begin(19200); // Set up a serial connection to the computer at 19200 baud rate
    setSyncProvider(requestSync);  // set function to call when sync required
    
    // Get time sync with computer
    while(true){
        if(Serial.available()){
            processSyncMessage(); // The Python script only sends data to the Arduino in the form of time-stamps
        }
        
        if(timeStatus() == timeNotSet) {
            delay(1000); // wait one second, then try again to receive a time-stamp from the computer
        }else{
            break; // if a time-stamp has been established, break out of the while loop
        }
    }
    
    delay(1000);
    Serial.println();
  
    // the current time in seconds since January 1st 1970, synced with the computer
    time_t t = now();
  
    // this is the beginning of any new OBMP JSON packet   
    preamble = "{\"sender\":{\"address\":\"" + address + "\",\"time-format\":\"millisec-rel\",\"start-time\":{\"time\":" + t + ",\"format\":\"sec-epoch\"}},\"data\":[{\"name\":";
  
    // the current time in milliseconds since starting the Arduino
    // time values in the OBMP packets are given as the time since this starting time
    last_time = millis();
}

void loop() {   // send the OBMP JSON data
  
    Serial << preamble;
    Serial << "\"" << sensor_name << "\"";
    Serial << ",\"vals\":[";
  
  
    // Read the sensor value and output the time-stamped values to the serial port
    int i = 1;
    int val; // sensor value
    for(i=1;i < num_samples;i++){
        time = millis() - last_time;
        val = analogRead(A0);
        Serial << "{\"time\":" << time << ",\"val\":" << val << "},";
    }
    
    // The last sensor sample in a packet
    time = millis() - last_time;
    val = analogRead(A0);
    Serial << "{\"time\":" << time << ",\"val\":" << val << "}]}]}";
    Serial.println();

}


// Obtain a time-stamp from the computer
void processSyncMessage() {
    unsigned long pctime;
    const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

    if(Serial.find(TIME_HEADER)) {
        pctime = Serial.parseInt();
        if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
            setTime(pctime); // Sync Arduino clock to the time received on the serial port
        }
    }
}


// Request a time-stamp from the computer
time_t requestSync(){
    Serial.write(TIME_REQUEST);  
    return 0;
}
