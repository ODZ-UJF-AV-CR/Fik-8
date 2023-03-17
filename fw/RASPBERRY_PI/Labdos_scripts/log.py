#!/usr/bin/env python3
# -*- coding: utf-8 -*- 
import time
import serial
import threading
import serial.tools.list_ports;
import sys

def handle_data(data, fn):
    print(data, end='')
    datafname = fn + '.log'
    with open(datafname, "a") as nbf:
        nbf.write(data)
    nbf.close()

def read_from_port(ser, fn):
	while True:
		reading = ser.readline().decode('utf-8', errors='replace').rstrip()
		if (len(reading) > 0):
			handle_data(str(time.time()) + ',' + reading + '\n', fn)


if (len(sys.argv) < 2):
    print('Please type measurement identification in the command-line.')
    exit(1)
    
print(sys.argv)

for port in serial.tools.list_ports.comports():
	print(port.device, port.vid)

	if (port.vid == 1027):
	    baud = 115200 # 9600, 115200
	    name = sys.argv[1] + '_' + port.name
	    
	    serial_port = serial.Serial(port.device, baud)
	    time.sleep(0.1)
	    serial_port.flush()

	    thread = threading.Thread(target=read_from_port, args=(serial_port,name))
	    thread.start()
