/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for USBtoSerial.c.
 */

#ifndef _DUAL_SERIAL_H_
#define _DUAL_SERIAL_H_

#ifdef __cplusplus
extern "C"{
#endif


	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/interrupt.h>
		#include <avr/power.h>
		#include <string.h>


		#include "Descriptors.h"
		
		/* #include <LUFA/Version.h> */
		
		#include <LUFA/Drivers/USB/USB.h>
 		#include <LUFA/Drivers/Peripheral/Serial.h>

	/* Macros: */
		/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
		#define LEDMASK_USB_NOTREADY      LEDS_LED1

		/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
		#define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

		/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
		#define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

		/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
		#define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

		
		
		
	/* Function Prototypes: */
		void SetupHardware(void);
		void lufaInit(void);
		void lufaLoop(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);



		void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo);
		void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo);

struct ringbuf {
	uint8_t *data;
	uint8_t mask;

      	/* XXX these must be 8-bit quantities to avoid race conditions. */
        uint8_t put_ptr, get_ptr;
};

extern int8_t opCode;

extern struct ringbuf serialRx_Buffer;
extern struct ringbuf USBtoUSART_Buffer;
extern struct ringbuf USARTtoUSB_Buffer;
extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC0_Interface;

extern void setTcpRecieveCallback(void (*fun)(void));

#ifdef __cplusplus
}
#endif



#endif

