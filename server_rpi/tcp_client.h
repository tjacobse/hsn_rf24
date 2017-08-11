/**
    TCP Client class
*/

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent

using namespace std;

class tcp_client
{
 private:
  unsigned int debug;
  int sock;
  std::string address;
  int port;
  struct sockaddr_in server;

 public:
  tcp_client(unsigned int debug);
  bool conn(string, int);
  bool send_data(string data);
  string receive(int);
};
