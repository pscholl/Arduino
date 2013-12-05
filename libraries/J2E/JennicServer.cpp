extern "C" {
#include "string.h"
}

#include "JennicClass.h"
#include "JennicClient.h"
#include "JennicServer.h"

JennicServer::JennicServer(uint16_t port)
{
  _port = port;
}

void JennicServer::begin()
{
	// send opcode 19 = 0x13
	Jennic.write(0x13);

	// send payload length
	Jennic.write(0x02);

	//send port
	Jennic.write((uint8_t) _port >> 8);
	Jennic.write((uint8_t) _port);

	while(Jennic.available() < 1)
		;

	Jennic.read();
	opCode = -1;

}


JennicClient JennicServer::available()
{
  // TODO if someone has connected to the Server an written Data return an
  // ClientEthernet Object with one (not always the same!) corresponding 
  // connection. If no one is there return an EthernetClient(-1) Object. 
  	
	// send opcode 20 = 0x14
  	Jennic.write(0x14);
	// send payload
	Jennic.write(0x02);
	// send port
	Jennic.write((uint8_t) _port >> 8);
	Jennic.write((uint8_t) _port);

	while(Jennic.available() < 19)
		;
	opCode = -1;
	uint16_t adr[8];
	for(uint8_t i = 0; i < 8; i++){
		adr[i] = (uint16_t) Jennic.read() << 8;
		adr[i] = (uint16_t) Jennic.read();
	}
	IPv6Address ip(adr);
	
	uint16_t port = 0;
	port = (uint16_t) Jennic.read() << 8;
	port = (uint16_t) Jennic.read();

	return new JennicClient(port, ip);
}

size_t JennicServer::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t JennicServer::write(const uint8_t *buffer, size_t size) 
{
  // TODO write buffer to all connected clients and return the number
  // of clients to which were written.
  	// write opcode 21 = 0x15
	Jennic.write(0x15);

	uint8_t s = (uint8_t) size;
	s += 2 // payload = size + 2bytes port
	// write payload length
	Jennic.write(s);

	//write port
	Jennic.write((uint8_t) _port >> 8);
	Jennic.write((uint8_t) _port);

	for(uint8_t i = 0; i < (uint8_t) size; i++){
		Jennic.write(buf[i]);
	}
	
	while(Jennic.available() < 1)
		;
	
	Jennic.read();
	opCode = -1;

  	return 1;
}
