
extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"



uint16_t EthernetClient::_destPort;
uint16_t EthernetClient::_localPort;

IPv6Address EthernetClient::_destIp;

EthernetClient::EthernetClient(){
	_localPort = 0;
}

EthernetClient::EthernetClient(uint16_t localPort) {
	// TODO: make sure there is an option to create a "dead" EthernetClient 
	// in case the Server has to return an EthernetClient Object and no one
	// has connected.
	//
	// this Constructor ist used if the EthernetServer 
	// returns an EthernetClient Object.
	_localPort = localPort;
}
// Destructor
EthernetClient::~EthernetClient() {
	// TODO: abort Connection
}

int EthernetClient::connect(const char* host, uint16_t port) {
  	_destPort = port;
	
	// send opcode
  	Jennic.write(0x0C);
	
	// send payload length
  	for (uint8_t i = 0;host[i]; i++)
         ;
  	Jennic.write(i+2);

	// send port
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	// send host
  	for (i = 0;host[i]; i++)
  		Jennic.write(host[i];

	// wait for response
	while(Jennic.available() < 17)
		;
	
	// OPcode and payload length is ok?
	if(Jennic.read() == 0x0C && Jennic.read() == 0x11){
		
		// write ipAddress in a local variable
		for(uint8_t i = 0; i < 8)
			_destIp[i] == Jennic.read();
			_destIp[i] << 8;
			_destIp[i] == Jennic.read();
		}

		if(Jennic.read() == 0x01)
			return true;
	}
	return false;
}

int EthernetClient::connect(IPv6Address ip, uint16_t port) {
	// Send Jennic Connect command
  	Jennic.write(0x0D);
  	Jennic.write(0x12);

	_destIp = ip;
	_destPort = port;

	// send ip address
	for(uint8_t i = 0; i < 8){
		uint8_t tmp = ip[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) ip[i]);
	}

	// send port
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	// wait for response
	while(Jennic.available() < 3)
		;
	
	if(Jennic.read() == 0x16 && Jennic.read() == 0x01 && Jennic.read() == 0x01)
		return true;
	return false;
}

size_t EthernetClient::write(uint8_t b) {
	// send opcode and payload length
	Jennic.write(0x0E); // = dec 14
	Jennic.write(0x13); // = dec 19
	
	// send ip address
	for(uint8_t i = 0; i < 8){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	// send data
	Jennic.write(b);
	
	// wait for response
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();
}

size_t EthernetClient::write(const uint8_t *buf, size_t size) {
	// send opcode 	
	Jennic.write(0x0E);

	// send payload length
	uint8_t s = (uint8_t) size;
	s += 18;
	Jennic.write(s);

	// send ip address
	for(uint8_t i = 0; i < 8){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// sned port
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	// send data
	for(uint8_t i = 0; i < (uint8_t) size; i++)
		Jennic.write(buf[i]);
  	
	// wait for response
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();

	return size;
}
int EthernetClient::available() {
	// send opcode 	
	Jennic.write(0x0F);

	// send payload length
	Jennic.write(0x12);

	// send ip address
	for(uint8_t i = 0; i < 8){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// sned port
  	Jennic.write((uint8_t) _destPort >> 8);
  	Jennic.write((uint8_t) _destPort);

	// wait for response
	while(Jennic.available() < 3)
		;
	Jennic.read();
	Jennic.read();
	return Jennic.read();

}

int EthernetClient::read() {
	// send opcode 	
	Jennic.write(0x10); // = dec 16

	// send payload length
	Jennic.write(0x12); // = dec 18

	// send ip address
	for(uint8_t i = 0; i < 8){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write((uint8_t) _destPort >> 8);
  	Jennic.write((uint8_t) _destPort);
	
	// TODO
	// process the returned Data from the Jennic.
}
int EthernetClient::read(uint8_t *buf, size_t size) {
	// TODO
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
  // TODO: say Jennic to close the Connection
}

uint8_t EthernetClient::connected() {
	// TODO: return if Jennic has still a Connection 
	// OR there ist still something to read (peek for it).
	// (atm Connection and Data is dropped by the Jennic if
	// connection closes)
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
