#include "Ethernet.h"

#include "ringbuf.c"


int SerialMux::newSock(){
// TODO 
	return -1;
}


int SerialMux::rmSock(uint8_t socket){
// TODO 
	return -1;
}

int SerialMux::write(uint8_t socket, uint8_t opcode, uint8_t[] payload, uint8_t size){
// TODO 
	return -1; // returns the size
}
int SerialMux::write(uint8_t socket, uint8_t opcode, uint8_t payload){
	return write(socket, opcode &payload, 1); // returns the size
}

void SerialMux::read(uint8_t socket, uint8_t[] payload, uint8_t size){
// TODO
}

SerialMux mux;
