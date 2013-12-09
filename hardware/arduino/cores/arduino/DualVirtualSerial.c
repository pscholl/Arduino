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
 *  Main source file for the USBtoSerial project. This file contains the main tasks of
 *  the project and is responsible for the initial application hardware configuration.
 */
#include "atmel_bootloader.h"
#include "Arduino.h"
#include "DualVirtualSerial.h"
#include <util/delay.h>
#include "ringbuf.c"

/* missing on atmega32u4 */
#define UCSR1D _SFR_MEM8(0xCB)
#define RTSEN 0
#define CTSEN 1

#define JEN_RESETN  _BV(7) /* PC7 */
#define JEN_SPIMISO _BV(3) /* PB3 */
#define JEN_SPIMOSI _BV(2) /* PB2 */
#define JEN_CLOCK   _BV(1) /* PB1 */
#define JEN_CTS     _BV(0) /* PF0 */
#define JEN_SPISS   _BV(6) /* PE6 */

#define PWR_BUCKON  _BV(1) /* PF1, on by default */
#define PWR_STBY    _BV(4) /* PF4, pull low! */
#define PWR_BUTTON  _BV(4) /* PB4, input pin */
#define PWR_SENSORS _BV(6) /* PC6, on by default, toggle for reset */

static uint8_t usbtouart[128], uarttousb[128], serialRx[128]; // need size of power two!
struct ringbuf USARTtoUSB_Buffer, USBtoUSART_Buffer;
struct ringbuf serialRx_Buffer;
/** used to toggle the reset lines of the Jennic module, entering programming
 * mode and resetting */
static volatile bool jennic_reset_event = false;
// static volatile uint8_t cdc2_event = 0;

/** actual jennic mode */
static bool jennic_in_programming_mode = false;

/** Pointer to Callback-Function which is called if Jennic recieved something */
void (*callback)(void);
void nothing(void); // TODO remove
void setTcpRecieveCallback(void(*fun)(void));

/** command decode */
int8_t opCode = -1;

/** handle incoming Data on the Arduino CDC tty */
void CDC_Arduino_In_Task(void);

/** change serial configuration between atmega and jennic */
void Serial_Config(uint32_t Baudrate, uint8_t DataBits, uint8_t StopBits, uint8_t Parity);
/** restart jennic and set to programming or normal mode */
void Jennic_Set_Mode(bool programming);
/** handle incoming serial data from jennic */
void Jennic_In_Task(void);
/** handle incoming data on the Jennic CDC tty */
void CDC_In_Task(void);

/** CDC Interface for Arduino
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC0_Interface =
{
  .Config =
 {
    .ControlInterfaceNumber   = 0,
    .DataINEndpoint           =
    {
      .Address                = CDC1_TX_EPADDR,
      .Size                   = CDC1_TXRX_EPSIZE,
      .Banks                  = 2,
    },
    .DataOUTEndpoint                =
    {
      .Address                = CDC1_RX_EPADDR,
      .Size                   = CDC1_TXRX_EPSIZE,
      .Banks                  = 2,
    },
    .NotificationEndpoint           =
    {
      .Address                = CDC1_NOTIFICATION_EPADDR,
      .Size                   = CDC_NOTIFICATION_EPSIZE,
      .Banks                  = 2,
    },
  },
};
/* CDC Interface for Jennic
 **/
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC1_Interface =
{
  .Config =
  {
    .ControlInterfaceNumber   = 2,
    .DataINEndpoint           =
    {
      .Address                = CDC2_TX_EPADDR,
      .Size                   = CDC2_TXRX_EPSIZE,
      .Banks                  = 2,
    },
    .DataOUTEndpoint                =
    {
      .Address                = CDC2_RX_EPADDR,
      .Size                   = CDC2_TXRX_EPSIZE,
      .Banks                  = 2,
    },
    .NotificationEndpoint           =
    {
      .Address                = CDC2_NOTIFICATION_EPADDR,
      .Size                   = CDC_NOTIFICATION_EPSIZE,
      .Banks                  = 2,
    },
  },
};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
void lufaInit(void)
{
  /* activate specific usb config */
  SetupHardware();
  USB_Init();
  
  //CDC_Device_CreateStream(&VirtualSerial_CDC0_Interface, &stdout);

  ringbuf_init(&USARTtoUSB_Buffer, uarttousb, sizeof(uarttousb));
  ringbuf_init(&USBtoUSART_Buffer, usbtouart, sizeof(uarttousb));

  ringbuf_init(&serialRx_Buffer, serialRx, sizeof(serialRx));
  
  callback = nothing;

  /* restart jennic and set to normal mode. XXX needs serial line ops */
  Serial_Config(1000000, 8, CDC_LINEENCODING_OneStopBit, CDC_PARITY_None);
  Jennic_Set_Mode(false);

}
void lufaLoop(void)
{
	if(!jennic_in_programming_mode){
		CDC_Device_USBTask(&VirtualSerial_CDC0_Interface);
		CDC_Arduino_In_Task();
	}
    do{
  	  if (jennic_reset_event)
  	  {
  	    
  	    if (!jennic_in_programming_mode)
  	    {
  	      jennic_in_programming_mode = true;
  	      Serial_Config(38400, 8, CDC_LINEENCODING_OneStopBit, CDC_PARITY_None);
  	      Jennic_Set_Mode(true); /* pull jennic into programming mode */
  	    }
  	    else
  	    {
  	      jennic_in_programming_mode = false;
  	      Serial_Config(1000000, 8, CDC_LINEENCODING_OneStopBit, CDC_PARITY_None);
  	      Jennic_Set_Mode(false); /* pull jennic into normal mode */

  	    }
  	    jennic_reset_event = false;
  	  }

  	  //if(jennic_in_programming_mode){
  	  Jennic_In_Task();
  	  CDC_In_Task();
  	  CDC_Device_USBTask(&VirtualSerial_CDC1_Interface);
	  
  	  USB_USBTask();
  	  /* do house-keeping */
  	  
	  // UDRE Interrupt enable (USART)
	  // cause if our USB->USART ringbuf is full we disable the interrupt.
	  if ( ringbuf_elements(&USBtoUSART_Buffer) && !(UCSR1B & (1 << UDRIE1)) )
  	    UCSR1B |= (1 << UDRIE1);
  	}while(jennic_in_programming_mode);


}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* enable clock division, run on 16MHz */
  clock_prescale_set(clock_div_1);

  /* enable Interrupts */
  GlobalInterruptEnable();

  /* disable JTAG since we use the pins for I/O, e.g. the STBY pin */
  JTAG_DISABLE();

  /* pull STBY low, so enough power can be provided */
  DDRF  |=  (PWR_STBY);
  PORTF &= ~(PWR_STBY);

  /* reset sensors */
  DDRC  |= PWR_SENSORS;
  PORTC &= ~(PWR_SENSORS);

  /* enable pushbutton */
  DDRB |= ~(PWR_BUTTON);
  
  /* set default uart configuration */
  Serial_Config(1000000, 8, CDC_LINEENCODING_OneStopBit, CDC_PARITY_None);

  /* get off the spi-bus */
  DDRB &= ~JEN_SPIMISO;
  DDRB &= ~JEN_SPIMOSI;
  DDRB &= ~JEN_CLOCK;
  DDRE &= ~JEN_SPISS;
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
  /* configure specific interfaces */
  CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC0_Interface);
  CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC1_Interface);

}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC0_Interface);
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC1_Interface);
}

/** ISR to manage the reception of data from the serial port, placing received bytes into a circular buffer
 *  for later transmission to the host.
 */
ISR(USART1_UDRE_vect, ISR_BLOCK)
{
  if (ringbuf_elements(&USBtoUSART_Buffer))
    UDR1 = ringbuf_get(&USBtoUSART_Buffer);
  else
    UCSR1B &= ~(1 << UDRIE1);
}

ISR(USART1_RX_vect, ISR_BLOCK)
{
  uint8_t receivedByte;
  // Obsolete bc ringbuf_put drops it if buf is full.
  // if (ringbuf_elements(&USARTtoUSB_Buffer) >= ringbuf_size(&USARTtoUSB_Buffer) - 1 )
  //  return;

  receivedByte = UDR1;

  // TODO check for callback Code
  // set var if waiting for normal reply.
  // if not awaiting normal reply and receivedByte == 42 call callback.
  if(!jennic_in_programming_mode && opCode < 0){
  	if(receivedByte == 42){
	  	callback();
		return;
  	}
	opCode = receivedByte;
	return;
  }

  // removed if condition cause we need the serial in even if theres no USB Connection
  // if (USB_DeviceState == DEVICE_STATE_Configured) {
    ringbuf_put(&USARTtoUSB_Buffer, receivedByte);
  // }
}

/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
 {

	if(CDCInterfaceInfo->Config.ControlInterfaceNumber == 2 && jennic_in_programming_mode)
	{
  	/* only allowed in programming mode through programmer (jenprog) */
    	Serial_Config(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS, 
            CDCInterfaceInfo->State.LineEncoding.DataBits, 
            CDCInterfaceInfo->State.LineEncoding.CharFormat, 
            CDCInterfaceInfo->State.LineEncoding.ParityType);
	}
}

/** Event handler for the CDC Class driver Host-to-Device Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{

	//CDC_Device_SendString(&VirtualSerial_CDC0_Interface, "CDC_Device_ControlLineStateChanged");
	if(CDCInterfaceInfo->Config.ControlInterfaceNumber == 2)
        {
	//CDC_Device_SendString(&VirtualSerial_CDC0_Interface, "ContlEv Int2\n");
	/* fire a reset jennic event if there is a high-low-high transition within
  	 * SWITCH_TIMEOUT ms on the dtr line */
  		// static clock_time_t previous_change = 0;
  		static bool currentDTRState = false;

		static unsigned long jennic_reset_time = 0;
  	/* dtr line changed? */
  		if ((CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != currentDTRState)
  		{
    			unsigned long delay = millis()-jennic_reset_time;
    			currentDTRState    = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);
			
			if(delay < 250 && currentDTRState){
				jennic_reset_event = true;
			}
			//jennic_reset_event = currentDTRState && (delay < 250);
    			jennic_reset_time    = millis(); /* returns the usb frame clock in ms */
  		}
	}
	else if(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS==1200)
        {
        	bool currentDTRState  = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);

		if (!currentDTRState && millis() > 5000)
                        run_bootloader();

		
	}
}

/** change serial configuration between atmega and jennic */
void Serial_Config(uint32_t Baudrate, uint8_t DataBits, uint8_t StopBits, uint8_t Parity)
{
  uint8_t ConfigMask = 0;

  switch (Parity)
  {
    case CDC_PARITY_Odd:
      ConfigMask = ((1 << UPM11) | (1 << UPM10));
      break;
    case CDC_PARITY_Even:
      ConfigMask = (1 << UPM11);
      break;
  }

  if (StopBits == CDC_LINEENCODING_TwoStopBits)
    ConfigMask |= (1 << USBS1);

  switch (DataBits)
  {
    case 6:
      ConfigMask |= (1 << UCSZ10);
      break;
    case 7:
      ConfigMask |= (1 << UCSZ11);
      break;
    case 8:
      ConfigMask |= ((1 << UCSZ11) | (1 << UCSZ10));
      break;
  }

  /* Must turn off USART before reconfiguring it, otherwise incorrect operation may occur */
  UCSR1B = 0;
  UCSR1A = 0;
  UCSR1C = 0;

  /* Set the new baud rate before configuring the USART */
  UBRR1  = SERIAL_2X_UBBRVAL(Baudrate);
  UCSR1A = (1 << U2X1);

  UCSR1C = ConfigMask;
  UCSR1B = ((1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1));

  /* enable RTS/CTS flow control */
  UCSR1D = (1<<RTSEN)|(1<<CTSEN);
}

/** restart jennic and set to programming or normal mode */
void Jennic_Set_Mode(bool programming)
{
  //SPI_Disable();
  SPCR &= ~_BV(SPE);


  PORTB &= ~(JEN_SPIMOSI);
  PORTC &= ~(JEN_RESETN);
   DDRC |= JEN_RESETN;
   DDRB |= JEN_SPIMISO;
  delay(5);

  if (programming)
  {
    DDRC &= ~JEN_RESETN;
    delay(10);
    DDRB &= ~JEN_SPIMISO;
  }
  else
  {
    DDRB &= ~JEN_SPIMISO;
    delay(10);
    DDRC &= ~JEN_RESETN;
  }


}

void CDC_Arduino_In_Task()
{
uint16_t bytes = CDC_Device_BytesReceived(&VirtualSerial_CDC0_Interface);
	while(bytes--){
	/* Read bytes from the USB OUT endpoint and store it for the Arduino Serial Class */
 		if ( ringbuf_elements(&serialRx_Buffer) < ringbuf_size(&serialRx_Buffer)-2 ) {
   			int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC0_Interface);
   			if ( !(ReceivedByte < 0) )
     				ringbuf_put(&serialRx_Buffer, ReceivedByte);
 		}
 		else{
 			return;
 		}
 	} // end while
}

void CDC_In_Task()
{
 /* Read bytes from the USB OUT endpoint and transmit to jennic if programming mode */
 if ( ringbuf_elements(&USBtoUSART_Buffer) < ringbuf_size(&USBtoUSART_Buffer)-2 ) {
   // TODO check int16_t type
   int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC1_Interface);
   if ( !(ReceivedByte < 0) )
     ringbuf_put(&USBtoUSART_Buffer, ReceivedByte);
 }
}

void Jennic_In_Task()
{
  size_t i;

  Endpoint_SelectEndpoint(VirtualSerial_CDC1_Interface.Config.DataINEndpoint.Address);
  if ( !Endpoint_IsINReady() )
    return;

  /* Read bytes from the USART receive buffer and send with CDC */
  /* pass through in programming mode */
  if (ringbuf_elements(&USARTtoUSB_Buffer) && jennic_in_programming_mode)
  {
    uint8_t byte = ringbuf_get(&USARTtoUSB_Buffer);

      if (CDC_Device_SendByte(&VirtualSerial_CDC1_Interface, byte) != ENDPOINT_READYWAIT_NoError)
        return;
  }
}

void nothing(){
}

void setTcpRecieveCallback(void(*fun)(void)){
	callback = fun;
}
