
extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "JennicClass.h"
#include "JennicClient.h"
#include "JennicServer.h"



//uint16_t JennicClient::_destPort;
//uint16_t JennicClient::_localPort;

//IPv6Address JennicClient::_destIp;

JennicClient::JennicClient(){
	_localPort = 0;
	_destPort = 0;
}

JennicClient::JennicClient(uint16_t localPort) {
	// TODO: make sure there is an option to create a "dead" JennicClient 
	// in case the Server has to return an JennicClient Object and no one
	// has connected.
	//
	// this Constructor ist used if the EthernetServer 
	// returns an JennicClient Object.
	_localPort = localPort;
}
// Destructor
JennicClient::~JennicClient() {
	// TODO: abort Connection
}

int JennicClient::connect(const char* host, uint16_t port) {
  	_destPort = port;
	
	// send opcode
  	Jennic.write(0x0C);

	uint8_t i = 0;
	// send payload length
  	for (i = 0; host[i]; i++)
         ;
  	Jennic.write(i+2);
	//strlen
	
	// send port
  	Jennic.write((uint8_t) port >> 8);
  	Jennic.write((uint8_t) port);
	
	// send host
  	for (i = 0; host[i]; i++)
  		Jennic.write(host[i]);

	// wait for response
	while(Jennic.available() < 17)
		;
	
	// OPcode and payload length is ok?
	if(Jennic.read() == 0x0C && Jennic.read() == 0x11){
		
		// write ipAddress in a local variable
		for(i = 0; i < 8; i++){
			_destIp[i] == Jennic.read();
			_destIp[i] << 8;
			_destIp[i] == Jennic.read();
		}

		if(Jennic.read() == 0x01)
			return true;
	}
	return false;
}

int JennicClient::connect(IPv6Address ip, uint16_t port) {
	// Send Jennic Connect command
  	Jennic.write(0x0D);
  	Jennic.write(0x12);

	_destIp = ip;
	_destPort = port;

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
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

size_t JennicClient::write(uint8_t b) {
	// send opcode and payload length
	Jennic.write(0x0E); // = dec 14
	Jennic.write(0x13); // = dec 19
	
	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write((uint8_t) _destPort >> 8);
  	Jennic.write((uint8_t) _destPort);
	
	// send data
	Jennic.write(b);
	
	// wait for response
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();
}

size_t JennicClient::write(const uint8_t *buf, size_t size) {
	// send opcode 	
	Jennic.write(0x0E);

	// send payload length
	uint8_t s = (uint8_t) size;
	s += 18;
	Jennic.write(s);

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// sned port
  	Jennic.write((uint8_t) _destPort >> 8);
  	Jennic.write((uint8_t) _destPort);
	
	// send data
	for(uint8_t i = 0; i < (uint8_t) size; i++)
		Jennic.write(buf[i]);
  	
	// wait for response
	while(Jennic.available() < 2)
		;
	if(Jennic.read() == 0x0E && Jennic.read() == s)
		return size;
	
	return 0;
}
int JennicClient::available() {
	// send opcode 	
	Jennic.write(0x0F);

	// send payload length
	Jennic.write(0x12);

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
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
	if(Jennic.read() == 0x0F && Jennic.read() == 0x01)
		return Jennic.read();

	return -1;

}

int JennicClient::read() {
	// TODO fix!
	uint8_t byte = 0;
	return read(&byte, 1);
}

int JennicClient::read(uint8_t *buf, size_t size) {
	// send opcode 	
	Jennic.write(0x10); // = dec 16

	// send payload length
	Jennic.write(0x13); // = dec 19

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write((uint8_t) _destPort >> 8);
  	Jennic.write((uint8_t) _destPort);
	
	// read x byte
	uint8_t numBytes = (uint8_t) size;
	Jennic.write(numBytes);


	while(Jennic.available() < (2 + numBytes))
		;

	if(Jennic.read() == 0x10 && Jennic.read() == 0x01){
		for(uint8_t i = 0; i < numBytes; i++){
			buf[i] = Jennic.read();
		}

		return numBytes;
	}
	return -1;
}

int JennicClient::peek() {
	
	// send opcode 	
	Jennic.write(0x11); // = dec 17

	// send payload length
	Jennic.write(0x12); // = dec 18

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write((uint8_t) _destPort >> 8);
  	Jennic.write((uint8_t) _destPort);

	// wait for response
	while(Jennic.available() < 2)
		;
	if(Jennic.read() == 0x11){
		// if theres nothing to peek payload length i 0x00
		if (Jennic.read() == 0x00)
			return -1;

		while(Jennic.available() < 1)
			;
		
		return Jennic.read();
	}
  	
  	return -1;
}

void JennicClient::flush() {
	while (available())
		read();
}

void JennicClient::stop() {
	// TODO: say Jennic to close the Connection
}

uint8_t JennicClient::connected() {
	// TODO: return if Jennic has still a Connection 
	// OR there ist still something to read (peek for it).
	// (atm Connection and Data is dropped by the Jennic if
	// connection closes)
	return -1;
}

uint8_t JennicClient::status() {
	//if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
	//return W5100.readSnSR(_sock);
  
	// Unused until stop() has changed!
	return -1;
}

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.

JennicClient::operator bool() {
  return available();
}
