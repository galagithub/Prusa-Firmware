#!/bin/python3

# serial port
port='/dev/ttyUSB0'

# serial number, 19 chars, must start with CZPX
serialNumber=bytearray(b'CZPXCOMPATMK2')

import serial
import random
snrnd = bytearray(str(random.randrange(999999)),'utf-8')
print("Random serial:", bytes(snrnd))
serialNumber+=snrnd
print("Full SN: ",bytes(serialNumber))
ser = serial.Serial()
ser.port = port
ser.baudrate = 115200
print("Opening port ",ser.name," ...")
try:
    ser.open()  # open serial port
    if ser.is_open:
        print("Looking for invalid SN start ....")
        bline = ser.readline()
        print(bline)
        line = bline.decode("utf-8")
        if line.startswith(";S"):
            ser.write(serialNumber)
            print("SN was set to ",serialNumber)
        else:
            print("Could not find Prusa SN write prompt!")
        ser.close()
except Exception as e:
    print("Exception was: ",e)
    pass
#    print("Could not open port! SN write aborted...")