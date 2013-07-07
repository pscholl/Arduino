#ifndef serial_mux_h
#define serial_mux_h

#include <inttypes.h>
#define MAX_SOCK_NUM 4

class SerialMux {
private:

public:
  
  int newSock();
  int rmSock();
  int write(uint8_t socket, uint8_t opcode, uint8_t[] payload, uint8_t size);
  int write(uint8_t socket, uint8_t opcode, uint8_t payload);
  void read(uint8_t socket, uint8_t[] payload, uint8_t size){
  
};

extern SerialMux mux;

#endif
