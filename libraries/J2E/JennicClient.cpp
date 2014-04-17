
extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "JennicClass.h"
#include "JennicClient.h"
#include "JennicServer.h"

// TODO:
// - timeout when jennic doesn't send an answer.
// - make sure that Jennic.available() == 0 in case of failure

//uint16_t JennicClient::_destPort;
//uint16_t JennicClient::_localPort;

//IPv6Address JennicClient::_destIp;

JennicClient::JennicClient(){
	//_localPort = 0;
	_destPort = 0;
}

// JennicClient::JennicClient(uint16_t localPort) {
	// TODO: make sure there is an option to create a "dead" JennicClient 
	// in case the Server has to return an JennicClient Object and no one
	// has connected.
	//
	// this Constructor ist used if the EthernetServer 
	// returns an JennicClient Object.
	// _localPort = localPort;
// }

JennicClient::JennicClient(uint16_t desPort, IPv6Address destIp){
	_destPort = desPort;
	_destIp = destIp;
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
	while(Jennic.available() < 18)
		;
	
	// OPcode and payload length is ok?
	if(opCode == 0x0C && Jennic.read() == 0x11){
	
	
		
		// write ipAddress in a local variable
		for(i = 0; i < 8; i++){
			_destIp[i] == Jennic.read();
			_destIp[i] << 8;
			_destIp[i] == Jennic.read();
		}
		
		if(Jennic.read() == 0x01)
			opCode = -1;
			return true;
	}
	opCode = -1;
	return false;
}

int JennicClient::connect(IPv6Address ip, uint16_t port) {
	// Send Jennic Connect command
  	Jennic.write(0x0D);
  	Jennic.write(0x12);
  	
	//Serial.println(0x0D);
  	//Serial.println(0x12);

	_destIp = ip;
	_destPort = port;

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = ip[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) ip[i]);

		//Serial.println(tmp);
		//Serial.println((uint8_t) ip[i]);
	}

	// send port
  	Jennic.write(port >> 8);
  	Jennic.write(port);

	//Serial.println(port >> 8);
  	//Serial.println(port);
	
	// wait for response
	while(Jennic.available() < 2)
		;
	

	//Serial.println(opCode);
	//Serial.println(Jennic.read());
	//Serial.println(Jennic.read());
	//
	//return false;
	// opcode payloadlength ok/notok
	if(opCode == 0x0D & (Jennic.read() == 0x01) & (Jennic.read() == 0x01)){
		opCode = -1;
		return true;
	}
	opCode = -1;
	return false;
}

size_t JennicClient::write(uint8_t b) {
	return write(&b, 1);
}

size_t JennicClient::write(const uint8_t *buf, size_t size) {
	// send opcode 	
	Jennic.write(0x0E);

	//Serial.println(0x0E);

	// send payload length
	uint8_t s = (uint8_t) size;
	s += 18;
	Jennic.write(s);
	//Serial.println(s);

	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);

		//Serial.println(tmp);
		//Serial.println((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write(_destPort >> 8);
  	Jennic.write(_destPort);
  	
	//Serial.println(_destPort >> 8);
  	//Serial.println(_destPort);
	
	
	// send data
	for(uint8_t i = 0; i < (uint8_t) size; i++){
		Jennic.write(buf[i]);
		//Serial.println(buf[i]);
  	}
	//Serial.println("send write...");
	
	//while(true){
	//	if(Jennic.available()){
	//		Serial.println(opCode);
	//		Serial.println(Jennic.read());
	//	}
	//}

	// wait for response
	while(Jennic.available() < 2)
		;
	
	// Serial.println("received response...");
	// TODO check if Data was sent.
	if(opCode == 0x0E && Jennic.read() == 0x01){
		opCode = -1;
		return Jennic.read();
	}
	
	Jennic.read();
	opCode = -1;
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
	
	// send port
  	Jennic.write(_destPort >> 8);
  	Jennic.write(_destPort);
  	
	// wait for response
	while(Jennic.available() < 2)
		;
	
	
	//Serial.println("-----------start------------");
	//Serial.println(opCode);
	//Serial.println(Jennic.read());
	//Serial.println(Jennic.read());
	//Serial.println(Jennic.read());
	//Serial.println(Jennic.read());
	//Serial.println("------------end-------------");


	if(opCode == 0x0F && Jennic.read() == 0x01){
		//Serial.println("opcode 0x0F answered");
		opCode = -1;
		int8_t a = Jennic.read();
		//Serial.print("bytes available: ");
		//Serial.println(a);
		return a;
	}

	Jennic.read();
	opCode = -1;
	return -1;

}

int JennicClient::read() {
	// TODO check!
	uint8_t byte = NULL;
	read(&byte, 1);
	return byte;
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
  	Jennic.write(_destPort >> 8);
  	Jennic.write(_destPort);
	
	// read x byte
	uint8_t numBytes = (uint8_t) size;
	Jennic.write(numBytes);

	//Serial.println("send read params to jennic");

	while(Jennic.available() < 1)
		;

	//Serial.println("Jennic answered read call");
	//Serial.print("opcode was: ");
	//Serial.println(opCode);

	if(opCode == 0x10){
		numBytes = Jennic.read(); // #payload length bytes

		//Serial.print("Bytes to read: ");
		//Serial.println(numBytes);
		
		for(uint8_t i = 0; i < numBytes; i++){
			while(!Jennic.available())
				;
			buf[i] = Jennic.read();
		}
		opCode = -1;
		return numBytes;
	}
	opCode = -1;
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
  	Jennic.write(_destPort >> 8);
  	Jennic.write(_destPort);

	// wait for response
	while(Jennic.available() > 1)
		;

	if(opCode == 0x11){
		
		// if theres nothing to peek payload length i 0x00
		if (Jennic.read() == 0x00){
			opCode = -1;
			return -1;
		}

		while(Jennic.available() < 1)
			;
		
		opCode = -1;
		return Jennic.read();
	}
  	opCode = -1;
  	return -1;
}

void JennicClient::flush() {
	while (available())
		read();
}

void JennicClient::stop() {
	Jennic.write(0x16);
	Jennic.write(0x12);
	
	// send ip address
	for(uint8_t i = 0; i < 8; i++){
		uint8_t tmp = _destIp[i] >> 8;
		Jennic.write(tmp);
		Jennic.write((uint8_t) _destIp[i]);
	}
	
	// send port
  	Jennic.write(_destPort >> 8);
  	Jennic.write(_destPort);
	

	// wait for response
	while(Jennic.available() > 1)
		;

	if(opCode == 0x16 && Jennic.read() == 0x00){
  		opCode = -1;
		return;
	}

  	opCode = -1;
  	return;
}


uint8_t JennicClient::connected() {
	// TODO: return if Jennic has still a Connection 
	// OR there ist still something to read (peek for it).
	// (atm Connection and Data is dropped by the Jennic if
	// connection closes) if(available() == -1)
	//	return 0;
	//return (available() > -1) ? true : false;
	if(available() > -1)
		return true;
	return false;
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
	if(available() > -1)
		return true;
	return false;
}
