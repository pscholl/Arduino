
#include <Arduino.h>
#include <IPv6Address.h>

IPv6Address::IPv6Address()
{
    memset(_address, 0, sizeof(_address));
}


IPv6Address::IPv6Address(uint16_t first_group, uint16_t second_group, uint16_t third_group, uint16_t fourth_group, uint16_t fifth_group, uint16_t sixth_group, uint16_t seventh_group, uint16_t eighth_group)
{
    _address[0] = first_group;
    _address[1] = second_group;
    _address[2] = third_group;
    _address[3] = fourth_group;
    _address[4] = fifth_group;
    _address[5] = sixth_group;
    _address[6] = seventh_group;
    _address[7] = eighth_group;
}


Ipv6Address::IPv6Address(const uint8_t *address)
{
    memcpy(_address, address, sizeof(_address));
}

IPv6Address& IPv6Address::operator=(const uint16_t *address)
{
    memcpy(_address, address, sizeof(_address));
    return *this;
}

bool IPv6Address::operator==(const uint16_t* addr)
{
    return memcmp(addr, _address, sizeof(_address)) == 0;
}

size_t IPv6Address::printTo(Print& p) const
{
    size_t n = 0;
    for (int i = 0; i < 7; i++)
    {
        n += p.print(_address[i], DEC);
        n += p.print(':');
    }
    n += p.print(_address[7], DEC);
    return n;
}

