#!/usr/bin/env python

from __future__ import print_function
import time
from struct import *
from RF24 import *
from RF24Network import *

# CE Pin, CSN Pin, SPI Speed
# Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
radio = RF24(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ)
network = RF24Network(radio)

millis = lambda: int(round(time.time() * 1000))
octlit = lambda n:int(n, 8)

# Address of our node in Octal format (01, 021, etc)
this_node = octlit("00")

radio.begin()
time.sleep(0.1)
network.begin(90, this_node)    # channel 90
radio.printDetails()

while 1:
    network.update()
    while network.available():
        header, payload = network.read()
        print("payload length ", len(payload))
        print("Header: " bytes(header))
        print("Payload: " bytes(payload))
        #ms, number = unpack('<LL', bytes(payload))
        #print('Received payload ', number, ' at ', ms, ' from ', oct(header.from_node))
time.sleep(1)
