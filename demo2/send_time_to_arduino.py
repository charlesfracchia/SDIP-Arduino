'''
Receives OBMP data from Arduino and prints to the screen.
Sends the current time to the Arduino to sync timestamps for OBMP.

Instructions:

1) First run the Arduino code: e.g., OBMPduino_demo1_v2

2) Then run this script via 

python send_time_to_arduino.py

3) Follow the instructions printed to the terminal

4) Once you've set up the serial connection, the terminal should start printing OBMP with the correct start-time and relative time values
'''

import os
import serial
from serial.tools import list_ports
import time
import calendar
import json

TIME_HEADER = "T" # Header sent to Arduino with time-sync signal
TIME_REQUEST = 7  # ASCII bell character

def main():
    
    '''Determine the serial port to communicate with'''
    print "Which of these serial ports is the Arduino attached to?"
    ports = list_serial_ports()
    for i in range(len(ports)): # display the list of ports to the command line for the user
        print str(i) + " : " + ports[i]
    var = raw_input("Enter the number as 0 to N-1 where N is the number of serial ports: ")
    port = ports[int(var)]
    print port
    ser = serial.Serial(port, 19200) # establish a serial connection at 19200 baud rate
    
    '''Receive output from Arduino. If a time sync request, send the current time.
    Otherwise, process successive lines of output as JSON object, pretty-printing them to the command line.'''
    line = ""
    while(True):
        c = ser.read() # read a single character from the serial port
        if ord(c) == TIME_REQUEST: # do a time sync
            # Greenwich mean time
            gmtime_in_seconds = calendar.timegm(time.gmtime()) # Greenwich mean time in seconds
            time_str = TIME_HEADER + str(gmtime_in_seconds) + "\n" # time-sync response string to send to arduino
            ser.write(time_str) # print time_str
        else: # echo the serial port if it's not doing a time sync
            if c == "\n": # check for the end of a line
                if line[0] == "{": # this is a crude sanity-check test for correct JSON
                    j = json.loads(line) # convert string to JSON objsect
                    s = json.dumps(j, sort_keys=False, indent=4, separators=(',', ': ')) # pretty print JSON object as string
                    print s
                    print "\n"
                else: # ignore input that fails crude sanity-check test for correct JSON
                    # print line
                    # print "\n"
                    pass
                line = "" # start building up a new line of text from the serial input
            else:
                line += c # add the most-recently received character from the serial port to the line of text

'''Find all available serial ports on the computer. The Arduino will show up as a serial port.'''
def list_serial_ports():
    # Windows
    if os.name == 'nt':
        # Scan for available ports.
        available = []
        for i in range(256):
            try:
                s = serial.Serial(i)
                available.append('COM'+str(i + 1))
                s.close()
            except serial.SerialException:
                pass
        return available
    else:
        # Mac / Linux
        return [port[0] for port in list_ports.comports()]
        
if __name__ == '__main__':
    main()