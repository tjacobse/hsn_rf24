/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 Update 2014 - TMRh20
 */

/**
 * Based on an example from RF24Network and SimpleDHT
 * TRANSMITTER NODE
 * Every X seconds, send a payload to the receiver node. 
 * Sample rate depends on the chosen number of measurements per sample
 * Put together by tjacobse
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <SimpleDHT.h>

RF24 radio(7,8);                    // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 01;        // Address of our node in Octal format
const uint16_t other_node = 00;       // Address of the other node in Octal format

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;

struct payload_t {                  // Structure of our payload
  float temp;
  float humidity;
};

struct temp_t {
  int temperature;
  int humidity;
  float temperatureWeight;
  float humidityWeight;
};

temp_t temp_s[10];

unsigned int samplePnt = 0;
unsigned int sampleCount = 0;
unsigned int sampleMax = 10;

unsigned int loopCount = 0;
unsigned int loopMax = 15;
unsigned int loopWaitBeforeSampling = loopMax-sampleMax; // not used yet

// How often to sample?
const unsigned long interval = 1000; //ms  
unsigned long last_sent;             // When did we last send?

void setup(void)
{
  Serial.begin(9600);
  Serial.println("RF24 from: RF24Network/examples/helloworld_tx/");
  Serial.println("DHT11 from Adafruit");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop() {
  
  network.update();                          // Check the network regularly
  
  unsigned long now = millis();
  // Is it sample time?
  if ( now - last_sent >= interval  )
    {
      last_sent = now;
      loopCount = loopCount + 1;
  if(loopCount < loopWaitBeforeSampling)
	{
	  Serial.println("... continues to sleep ...");
	  return;
	}
      
  if(sampleCount < sampleMax)
	{
	  byte temperature = 0;
	  byte humidity = 0;
	  int failCode = 0;
	  failCode = dht11.read(pinDHT11, &temperature, &humidity, NULL);
	  if (failCode != 0) 
	    {
	      Serial.print("Read DHT11 failed (error ");
	      Serial.print(failCode);
	      Serial.println(")");
	    }
	  else 
	    {
	      Serial.print("Sample OK: ");
	      Serial.print((float)temperature); Serial.print(" *C, "); 
	      Serial.print((float)humidity); Serial.println(" %");
	      temp_s[samplePnt].humidity = (int)humidity;
	      temp_s[samplePnt].temperature = (int)temperature;
	      samplePnt = samplePnt + 1;
	      if(samplePnt >= sampleMax)
		{
		  samplePnt = 0;
		}
	      sampleCount = sampleCount +1;
	    }
	}
      if(loopCount>loopMax)
	{
	  // Time for averaging and transmission
	  float avgTemp = 0.0;
	  float avgHumid = 0.0;
	  float avgWTemp = 0.0;
	  float avgWHumid = 0.0;
	  int ii = samplePnt;
	  float uniWeight = 1.0/((float)sampleCount);
	  
	  for(int i = sampleCount; i>0; i--)
	    {
	      temp_s[ii].temperatureWeight = uniWeight;
	      temp_s[ii].humidityWeight = uniWeight;
	      avgTemp = avgTemp + temp_s[ii].temperature*temp_s[ii].temperatureWeight;
	      avgHumid = avgHumid + temp_s[ii].humidity*temp_s[ii].humidityWeight;
	      ii = ii-1;
	      if (ii < 0) 
		{
		  ii = sampleMax-1;
		}
	    } 
	  
	  Serial.print("Average: Temperature: ");
	  Serial.print(avgTemp);
	  Serial.print(" , humidity: ");
	  Serial.println(avgHumid);
	  
	  Serial.print("Sending ... ");
	  payload_t payload = { avgTemp, avgHumid};
	  RF24NetworkHeader header(/*to node*/ other_node);
	  bool ok = network.write(header,&payload,sizeof(payload));
	  if (ok)
	    Serial.println("Sending OK.");
	  else
	    Serial.println("Sending Failed.");

      loopCount = 0;
      sampleCount = 0;
	}
      

      
    }
}
