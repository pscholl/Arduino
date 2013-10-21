
extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"



uint16_t EthernetClient::_destPort;

IPv6Address EthernetClient::_destIp;

EthernetClient::EthernetClient(){
}

EthernetClient::EthernetClient(uint8_t sock) {
	// TODO: make sure there is an option to create a "dead" EthernetClient 
	// in case the Server has to return an EthernetClient Object and no one
	// has connected.
	
}
// Destructor
EthernetClient::~EthernetClient() {
}

int EthernetClient::connect(const char* host, uint16_t port) {
  
  	Jennic.write(0x0C);
  	uint8_t i;
  	for (i = 0;host[i]; i++)
         ;
  	Jennic.write(i+2);

  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
  	for (i = 0;host[i]; i++)
  		Jennic.write(host[i];


	while(Jennic.available() < 17)
		;
	
	// OPcode and length is ok?
	if(Jennic.read() == 0x0C && Jennic.read() == 0x11){
		
		while(Jennic.available() > 0){
			// TODO: write ipAddress in a local variable
			
		}

		if(Jennic.read() == 0x01)
			return true;
	}
	return false;
}

int EthernetClient::connect(IPv6Address ip, uint16_t port) {
  	Jennic.write(0x13);
  	Jennic.write(0x18);

	// TODO: Implement IPv6Address and send the IP

  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	while(Jennic.available() < 3)
		;
	
	if(Jennic.read() == 0x16 && Jennic.read() == 0x01 && Jennic.read() == 0x01)
		return true;
	return false;
}

size_t EthernetClient::write(uint8_t b) {
	Jennic.write(0x14);
	Jennic.write(0x13); // = dec 19
	
	// TODO write IP Address
	
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	Jennic.write(b);
}

size_t EthernetClient::write(const uint8_t *buf, size_t size) {
		
	Jennic.write(0x14);
	Jennic.write();
	
	uint8_t s = (uint8_t) size;
	s += 18;
	Jennic.write(s);

	// TODO write IP Address
	
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	for(uint8_t i = 0; i < (uint8_t) size; i++)
		Jennic.write(buf[i]);
  	
	return size;
}
int EthernetClient::available() {
	// TODO: ask Jennic how much the Jennic has recieved.	  
}

int EthernetClient::read() {
	// TODO: ask Jennic for incomming Data
}
int EthernetClient::read(uint8_t *buf, size_t size) {
}

int EthernetClient::peek() {
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!available())
    return -1;
  
  // TODO: ask Jennic for peeking one byte -> set b to result
  
  return b;
}

void EthernetClient::flush() {
  while (available())
    read();
}

void EthernetClient::stop() {
  // TOTO: say Jennic to close the Connection
}

uint8_t EthernetClient::connected() {
	// TODO: return if Jennic has still a Connection 
	// OR there ist still something to read (peek for it).
  return -1;
}

uint8_t EthernetClient::status() {
  //if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
  //return W5100.readSnSR(_sock);
  
  // Unused until stop() has changed!
  return -1;
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

EthernetClient::operator bool() {
  return available();
}
