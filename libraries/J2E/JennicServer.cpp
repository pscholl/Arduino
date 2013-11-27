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
  // TODO start listening on Jennic
}


JennicClient JennicServer::available()
{
  // TODO if someone has connected to the Server an written Data return an
  // ClientEthernet Object with one (not always the same!) corresponding 
  // connection. If no one is there return an EthernetClient(-1) Object. 
  
}

size_t JennicServer::write(uint8_t b) 
{
  return write(&b, 1);
}

size_t JennicServer::write(const uint8_t *buffer, size_t size) 
{
  // TODO write buffer to all connected clients and return the number
  // of clients to which were written.
  return -1;
}
