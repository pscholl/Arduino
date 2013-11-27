#ifndef jennic_server_h
#define jennic_server_h

#include "Server.h"

class JennicClient;

class JennicServer : 
public Server {
private:
  uint16_t _port;
public:
  JennicServer(uint16_t);
  JennicClient available();
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};

#endif
