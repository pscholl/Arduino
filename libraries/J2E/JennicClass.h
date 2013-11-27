#ifndef j2e_h
#define j2e_h

extern "C" {
#include "DualVirtualSerial.h"
}

#include <inttypes.h>
#include "IPv6Address.h"
#include "JennicClient.h"
#include "JennicServer.h"

#define MAX_SOCK_NUM 4

class JennicClass {
private:

public:
  
  // Initialise the Ethernet shield to use the provided MAC address and gain the rest of the
  // configuration through DHCP.
  // Returns 0 if the DHCP configuration failed, and 1 if it succeeded
  int begin(uint8_t *mac_address);
  void begin(uint8_t *mac_address, IPv6Address local_ip);
  void begin(uint8_t *mac_address, IPv6Address local_ip, IPv6Address dns_server);
  void begin(uint8_t *mac_address, IPv6Address local_ip, IPv6Address dns_server, IPv6Address gateway);
  void begin(uint8_t *mac_address, IPv6Address local_ip, IPv6Address dns_server, IPv6Address gateway, uint8_t subnet);
  int maintain();

  void setCallback(void (*fun)(void));

  IPv6Address localIP();
  IPv6Address subnetMask();
  IPv6Address gatewayIP();
  IPv6Address dnsServerIP();

  friend class JennicClient;
  friend class JennicServer;
};

extern JennicClass Ethernet;

#endif
