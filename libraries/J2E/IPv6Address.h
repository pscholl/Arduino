/*
 *
 * MIT License:
 * Copyright (c) 2011 Adrian McEwen
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * adrianm@mcqn.com 1/1/2011
 */

#ifndef IPv6Address_h
#define IPv6Address_h

#include <Printable.h>

// A class to make it easier to handle and pass around IP addresses

class IPv6Address : public Printable {
private:
    uint16_t _address[8];  // IPv6 address
    // Access the raw byte array containing the address.  Because this returns a pointer
    // to the internal structure rather than a copy of the address this function should only
    // be used when you know that the usage of the returned uint8_t* will be transient and not
    // stored.
    uint16_t* raw_address() { return _address; };

public:
    // Constructors
    IPv6Address();
    IPv6Address(uint16_t first_group, uint16_t second_group, uint16_t third_group, uint16_t fourth_group, uint16_t fifth_group, uint16_t sixth_group, uint16_t seventh_group, uint16_t eighth_group);
    //IPAddress(uint32_t address);
    IPv6Address(const uint16_t *address);

    // Overloaded cast operator to allow IPAddress objects to be used where a pointer
    // to a four-byte uint8_t array is expected
    //
    // operator uint32_t() { return *((uint32_t*)_address); };
    // bool operator==(const IPAddress& addr) { return (*((uint32_t*)_address)) == (*((uint32_t*)addr._address)); };
    bool operator == (const IPv6Address& addr) {
    	// TODO check code.
    	for(uint8_t i = 0; i < 8; i++){
		if(_address[i] != addr._address[i]) return false;
	}
	return true;
    }
    bool operator==(const uint16_t* addr);

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint16_t operator[](int index) const { return _address[index]; };
    uint16_t& operator[](int index) { return _address[index]; };

    // Overloaded copy operators to allow initialisation of IPAddress objects from other types
    IPv6Address& operator=(const uint16_t *address);
    //IPAddress& operator=(uint32_t address);

    virtual size_t printTo(Print& p) const;

    friend class EthernetClass;
    // friend class UDP;
    friend class Client;
    friend class Server;
    // friend class DhcpClass;
    // friend class DNSClient;
};

const IPv6Address INADDR_NONE(0,0,0,0,0,0,0,0);


#endif
