#include "socket.h"
extern "C" {
#include "string.h"
}

#include "Ethernet.h"
#include "EthernetClient.h"
#include "EthernetServer.h"

EthernetServer::EthernetServer(uint16_t port)
{
  _port = port;
}

void EthernetServer::begin()
{
  // TODO start listening on Jennic
}


EthernetClient EthernetServer::available()
{
  // TODO if someone has connected to the Server an written Data return an
  // ClientEthernet Object with one (not always the same!) corresponding 
  // connection. If no one is there return an EthernetClient(-1) Object. 
  
}

size_t EthernetServer::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t EthernetServer::write(const uint8_t *buffer, size_t size) 
{
  // TODO write buffer to all connected clients and return the number
  // of clients to which were written.
  return -1;
}
