#ifndef jennic_ethernetclient_h
#define jennic_ethernetclient_h
#include "Arduino.h"	
#include "Print.h"
#include "IPv6Client.h"
#include "IPv6Address.h"


class JennicClient : public Client {

public:
  JennicClient();
  JennicClient(uint16_t localPort, IPv6Address);
  ~JennicClient();

  uint8_t status();
  virtual int connect(IPv6Address ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();

  friend class JennicServer;
  
  using Print::write;

private:
  uint16_t _destPort;
  uint16_t _localPort;
  IPv6Address _destIp;
};

#endif
