

#ifndef __UARTTOJENNIC__
#define __UARTTOJENNIC__

extern "C" {
#include "DualVirtualSerial.h"
}

#include "Stream.h"
//================================================================================
//================================================================================
//	Serial over CDC (Serial1 is the physical port)
//
class UARTtoJennic : public Stream
{
	public:

		void begin(uint16_t baud_count);
		void end(void);

		int available(void);
		int peek(void);
		int read(void);
		void flush(void);
		size_t write(uint8_t);
		using Print::write; // pull in write(str) and write(buf, size) from Print
		operator bool();
};

extern UARTtoJennic Jennic;

#endif

