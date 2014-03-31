

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
#include "SerialCDC.h"
#include "ringbuf.c"
#include <avr/wdt.h>

//extern struct ringbuf serialRx_Buffer;
//extern struct USB_ClassInfo_CDC_Device_t VirtualSerial_CDC0_Interface;

//int _serialPeek = -1;


void SerialCDC::begin(uint16_t baud_count)
{
}

void SerialCDC::end(void)
{
}

int SerialCDC::available(void)
{
  return ringbuf_elements(&serialRx_Buffer);
}

int SerialCDC::peek(void)
{
  ringbuf *r = &serialRx_Buffer;
  if(((r->put_ptr - r->get_ptr) & r->mask) > 0) {
        unsigned char c = r->data[r->get_ptr];
    return c;
  } else {
    return -1;
  }
}

int SerialCDC::read(void)
{
  if(ringbuf_elements(&serialRx_Buffer) > 0)
  {
    unsigned char c =  ringbuf_get(&serialRx_Buffer);
    return c;
  }
  else{
    return -1;
  }
}

void SerialCDC::flush(void)
{
  // according to the Arduino Docs it just waits to
  // complete the outgoing transmissions.
  // CDC_Device_Flush(&VirtualSerial_CDC0_Interface);
  CDC_Device_USBTask(&VirtualSerial_CDC0_Interface);
  USB_USBTask();
}

size_t SerialCDC::write(u8 c)
{
  /* check first if we can send a packet to the host, otherwise
   * we might overwrite what's in the buffer */
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return 0;

  Endpoint_SelectEndpoint(VirtualSerial_CDC0_Interface.Config.DataINEndpoint.Address);

  uint8_t status;
  do {
    while (!Endpoint_IsINReady()) {
      lufaLoop();
    }

   status = CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, c);

    CDC_Device_USBTask(&VirtualSerial_CDC0_Interface);
    USB_USBTask();
  } while (status == ENDPOINT_READYWAIT_Timeout);

  return status == ENDPOINT_READYWAIT_NoError;
}

// This operator is a convenient way for a sketch to check whether the
// port has actually been configured and opened by the host (as opposed
// to just being connected to the host).  It can be used, for example, in 
// setup() before printing to ensure that an application on the host is
// actually ready to receive and display the data.
// We add a short delay before returning to fix a bug observed by Federico
// where the port is configured (lineState != 0) but not quite opened.
SerialCDC::operator bool() {
  if((VirtualSerial_CDC0_Interface.State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) == true) return true;
  else return false;
  //bool result = false;
  // TODO check if CDC Connection is opened on the host side.
  //return result;
}

SerialCDC Serial;

