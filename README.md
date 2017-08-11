# rfdht
Client and Server for sensor acqusition using arduino as client and rpi as server connected through RF24Network.

To get started:

$git clone RF24
$cd RF24
$make
$cd ..
$git clone RF24Network
$cd RF24Network
$make
$git clone rfdht

To make rpi server
$cd server_rpi
Add thingspeak channel API keys
$make

To get the arduino client up and running
$git clone SimpleDHT
Copy or download SimpleDHT into the Arduino library
Import the .ino file into a project folder in Arduino.
Copy or download RF24 and RF24Network folder into the Arduino 
library
