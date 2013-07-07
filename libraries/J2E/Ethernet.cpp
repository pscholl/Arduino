#include "Ethernet.h"

int EthernetClass::begin(uint8_t *mac_address)
{
  // TODO: Init with DHCP
  return -1;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
  IPAddress subnet(255, 255, 255, 0);
  begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	// TODO: Build an IP connection Handler on Jennic
}

int EthernetClass::maintain(){

	// TODO: Tell Jennic to renew DHCP Lease
  return -1;
}

IPAddress EthernetClass::localIP()
{
  // TODO: get Local IP Address from Jennic
  return null;
}

IPAddress EthernetClass::subnetMask()
{
  // TODO: get Subnet Address from Jennic
  return null;
}

IPAddress EthernetClass::gatewayIP()
{
  // TODO: get Gateway IP Address from Jennic
  return null;
}

IPAddress EthernetClass::dnsServerIP()
{
 // TODO: get DNS IP Address from Jennic
  return null;
}

EthernetClass Ethernet;
