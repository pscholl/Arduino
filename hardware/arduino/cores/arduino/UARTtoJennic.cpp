

/* Copyright (c) 2011, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "Platform.h"
#include "UARTtoJennic.h"
#include "ringbuf.c"
#include <avr/wdt.h>


void UARTtoJennic::begin(uint16_t baud_count)
{
	// standard is 1 Megabaud
}

void UARTtoJennic::end(void)
{
}

int UARTtoJennic::available(void)
{
	return ringbuf_elements(&USARTtoUSB_Buffer);
}

int UARTtoJennic::peek(void)
{
	ringbuf *r = &USARTtoUSB_Buffer;
	if(((r->put_ptr - r->get_ptr) & r->mask) > 0) {
    		unsigned char c = r->data[r->get_ptr];
		return c;
	} else {
		return -1;
	}
}

int UARTtoJennic::read(void)
{
	if(ringbuf_elements(&USARTtoUSB_Buffer) > 0)
	{
		unsigned char c =  ringbuf_get(&USARTtoUSB_Buffer);
		return c;
	}
	else{
		return -1;
	}
}


void UARTtoJennic::flush(void)
{
	// according to the Arduino Docs it just waits to
	// complete the outgoing transmissions.
	while(1){
		if (!ringbuf_elements(&USBtoUSART_Buffer)){
			delay(5);
			return;
		}
	}
	return;
}

size_t UARTtoJennic::write(u8 c)
{
	return ringbuf_put(&USBtoUSART_Buffer, c);
}


// This operator is a convenient way for a sketch to check whether the
// port has actually been configured and opened by the host (as opposed
// to just being connected to the host).  It can be used, for example, in 
// setup() before printing to ensure that an application on the host is
// actually ready to receive and display the data.
// We add a short delay before returning to fix a bug observed by Federico
// where the port is configured (lineState != 0) but not quite opened.
UARTtoJennic::operator bool() {
	// therefore no user configuration is allowed 
	// the configuration is always done.
	return true;
}


UARTtoJennic Jennic;

