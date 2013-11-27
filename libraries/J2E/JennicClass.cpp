#include "JennicClass.h" 

int JennicClass::begin(uint8_t *mac_address)
{
  	// TODO: Init with DHCP
  	// and set mac Adress
	
	// Set Mac Address
	Jennic.write(0x01);
	Jennic.write(0x08);
	for(uint8_t i = 0; i < 8; i++){
		Jennic.write(mac_address[i]);
	}

	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();
  	
	// turn DHCP on
	Jennic.write(0x06);
	Jennic.write((uint8_t)0x00);
	while(Jennic.available() < 3)
		;
	Jennic.read();
	Jennic.read();
	
	return Jennic.read();
}

void JennicClass::begin(uint8_t *mac_address, IPv6Address local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last grougroup '1'
  IPv6Address dns_server = local_ip;
  dns_server[7] = 1;
  begin(mac_address, local_ip, dns_server);
}

void JennicClass::begin(uint8_t *mac_address, IPv6Address local_ip, IPv6Address dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last group being '1'
  IPv6Address gateway = local_ip;
  gateway[7] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void JennicClass::begin(uint8_t *mac_address, IPv6Address local_ip, IPv6Address dns_server, IPv6Address gateway)
{
  begin(mac_address, local_ip, dns_server, gateway, 128);
}

void JennicClass::begin(uint8_t *mac, IPv6Address local_ip, IPv6Address dns_server, IPv6Address gateway, uint8_t subnet)
{
	// TODO: Build an IP connection Handler on Jennic
	
	
	// Set Mac Address
	Jennic.write(0x01);
	Jennic.write(0x08);
	for(uint8_t i = 0; i < 8; i++){
		Jennic.write(mac[i]);
	}

	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();

	// Set IP Address
	Jennic.write(0x02);
	Jennic.write(0x10);
	for(uint8_t i = 0; i < 16; i++){
		uint16_t block = local_ip[i];
		uint8_t tmp = (block >> 8);
		Jennic.write(tmp);
		tmp = (uint8_t) block;
		Jennic.write(tmp);
	}
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();

	// Set DNS-Server Address
	Jennic.write(0x03);
	Jennic.write(0x10);
	for(uint8_t i = 0; i < 16; i++){
		uint16_t block = dns_server[i];
		uint8_t tmp = (block >> 8);
		Jennic.write(tmp);
		tmp = (uint8_t) block;
		Jennic.write(tmp);
	}
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();

	// Set Gateway Address
	Jennic.write(0x05);
	Jennic.write(0x10);
	for(uint8_t i = 0; i < 16; i++){
		uint16_t block = gateway[i];
		uint8_t tmp = (block >> 8);
		Jennic.write(tmp);
		tmp = (uint8_t) block;
		Jennic.write(tmp);
	}
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();

	// Set Subnetmask
	Jennic.write(0x04);
	Jennic.write(0x01);
	Jennic.write(subnet);
	while(Jennic.available() < 2)
		;
	Jennic.read();
	Jennic.read();
}

void JennicClass::setCallback(void(*fun)(void)){
	setTcpRecieveCallback(fun);
}

int JennicClass::maintain(){
	// Renew DHCP lease
	Jennic.write(0x07);
	Jennic.write((uint8_t)0x00);
	while(Jennic.available() < 3)
		;
	Jennic.read();
	Jennic.read();
	
	return Jennic.read();
}

IPv6Address JennicClass::localIP()
{
  // TODO: get Local IP Address from Jennic
  return NULL;
}

IPv6Address JennicClass::subnetMask()
{
  // TODO: get Subnet Address from Jennic
  return NULL;
}

IPv6Address JennicClass::gatewayIP()
{
  // TODO: get Gateway IP Address from Jennic
  return NULL;
}

IPv6Address JennicClass::dnsServerIP()
{
 // TODO: get DNS IP Address from Jennic
  return NULL;
}

JennicClass Ethernet;
