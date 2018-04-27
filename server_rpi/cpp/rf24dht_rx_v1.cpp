
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <time.h>
#include <string>
#include <string.h>

#include <tcp_client.h>

/**
 * g++ -L/usr/lib main.cc -I/usr/include -o main -lrrd
 **/

// Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  

RF24Network network(radio);

// Address of our node in Octal format
const uint16_t this_node = 00;

struct payload_t {                  // Structure of our payload
  float temp;
  float humidity;
};

const unsigned int noOfChannels = 3;

struct channel_t {
  unsigned int node_id;
  char api_key[20];
} channels[noOfChannels];

//ts_context_t *ctx   = NULL;
char buffer [250];
char bufferData [50];

unsigned int n, i;

char* api_key;
unsigned int node_id;
unsigned int ret;
char host[] = "api.thingspeak.com";

char api_key0[] = "";
char api_key1[] = "";
char api_key2[] = "";


int main(int argc, char** argv) 
{

  //  channels[0] = {0, "GLT4425XB6VZIK8K"};
  channels[0] = {0};
  strncpy(channels[0].api_key, api_key0, sizeof(api_key0) - 1);
  
  channels[1] = {1};
  strncpy(channels[1].api_key, api_key1, sizeof(api_key1) - 1);
  
  channels[2] = {2};
  strncpy(channels[2].api_key, api_key2, sizeof(api_key2) - 1);

  // Refer to RF24.h or nRF24L01 DS for settings
  
  radio.begin();
  
  delay(5);
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  radio.printDetails();
  
  while(1)
    {
      network.update();
      while ( network.available() )
	{     // Is there anything ready for us?
	  
	  RF24NetworkHeader header;        // If so, grab it and print it out
	  payload_t payload;
	  network.read(header,&payload,sizeof(payload));
	  
    	  struct timeval tp;
          gettimeofday(&tp, NULL);
          long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds
          //std::cout << mslong << std::endl;

	  printf("\n %f Received payload from node %i temp=%0.1f, humidity=%0.1f", (double)tp.tv_sec, (int)header.from_node, (float)payload.temp, (float)payload.humidity);

	  for(i=0; i<noOfChannels; i++)
	    {
	      if (channels[i].node_id == (unsigned int)header.from_node)
		{
		  node_id = channels[i].node_id;
		  api_key = channels[i].api_key;
		  break;
		}
	    }
	  switch(node_id)
	    {
	    case 0:
	      cout << "Nothing to do here yet.\n";
	      break;
	    default:
	      n = sprintf(bufferData,"field1=%0.1f&field2=%0.1f", (float)payload.temp, (float)payload.humidity);
	      n = sprintf(buffer, "POST /update HTTP/1.1\nHost: api.thingspeak.com\nConnection: close\nX-THINGSPEAKAPIKEY: %s\nContent-Type: application/x-www-form-urlencoded\nContent-Length: %i\n\n%s\n\n", api_key, n, bufferData);
	      break;
	  }
	  
	  tcp_client c(0);
	  ret = c.conn(host , 80);
	  if (ret != 0)
	    {
	      c.send_data(buffer);
	      cout << "OK";
	    }
	  else
	    {
	      cout << "!OK";
	    }

	}
      delay(100);
    }
  return 0;
}

