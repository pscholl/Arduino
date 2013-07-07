
extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"

#include "SerialMux.h"

EthernetClient::EthernetClient(){
	_sock = mux.newSock();
}

EthernetClient::EthernetClient(uint8_t sock) {
	// TODO: make sure there is an option to create a "dead" EthernetClient 
	// in case the Server has to return an EthernetClient Object and no one
	// has connected.
	
	_sock = sock;
}

EthernetClient::~EthernetClient() {
	mux.rmSock(_sock);
}

int EthernetClient::connect(const char* host, uint16_t port) {
	if(_sock == -1)
		return -1;
  
  // TODO: send Jennic Connect Command to port@host
   
  // mux.write(socket, opcode, payload, size)
  
  return -1;
}

int EthernetClient::connect(IPAddress ip, uint16_t port) {
	if(_sock == -1)
		return -1;
   // TODO: send Jennic Connect Command to port@ip
  return -1;
}

size_t EthernetClient::write(uint8_t b) {
	if(_sock == -1)
		return -1;
		
  // TODO: send Jennic write Command.
  // We assume sending Data never fails. Because Jennic might
  // Buffer this write with the next write Call.
   
  // mux.write(socket, opcode, payload, size)	// send Data
  return -1;
}

size_t EthernetClient::write(const uint8_t *buf, size_t size) {
	if(_sock == -1)
		return -1;
		
  // mux.write(socket, opcode, payload, size)	// send Data
  
}
int EthernetClient::available() {
  if(_sock == -1)
	return -1;

	// TODO: ask Jennic how much the Jennic has recieved.	  
  return -1;
}

int EthernetClient::read() {
	if(_sock == -1)
		return -1;
	
	// TODO: ask Jennic for incomming Data
	// mux.read(socket, *payload, size)	// get Data
  
	return -1;
}
int EthernetClient::read(uint8_t *buf, size_t size) {
	if(_sock == -1)
		return -1;
	// TODO: ask Jennic for incomming Data
  return -1;
}

int EthernetClient::peek() {
	if(_sock == -1)
		return -1;
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!available())
    return -1;
  
  // ask Jennic for peeking one byte -> set b to result
  
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
	if(_sock == -1)
		return -1;
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
