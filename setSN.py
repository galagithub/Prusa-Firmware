#!/bin/python3

port='/dev/ttyUSB0'
# serial number, 19 chars, must start with CZPX
serialNumber=b'CZPXCOMPATMK2000001'

import serial
ser = serial.Serial(port, 115200)  # open serial port
print(ser.name)         # check which port was really used
line = ser.readline()
print(line)
if line.startwith(";S"):
    ser.write(serialNumber)     # write a string
ser.close() 