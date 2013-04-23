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

static struct ringbuf USARTtoUSB_Buffer, USBtoUSART_Buffer;
static uint8_t usbtouart[128], uarttousb[128]; // need size of power two!

/** used to toggle the reset lines of the Jennic module, entering programming
 * mode and resetting */
static volatile bool jennic_reset_event = false;


/** actual jennic mode */
static bool jennic_in_programming_mode = false;


/** change serial configuration between atmega and jennic */
void Serial_Config(uint32_t Baudrate, uint8_t DataBits, uint8_t StopBits, uint8_t Parity);
/** restart jennic and set to programming or normal mode */
void Jennic_Set_Mode(bool programming);

/** handle incoming serial data from jennic */
void Jennic_In_Task(void);
/** handle incoming data on the CDC tty */
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
      .Banks                  = 1,
    },
    .DataOUTEndpoint                =
    {
      .Address                = CDC1_RX_EPADDR,
      .Size                   = CDC1_TXRX_EPSIZE,
      .Banks                  = 1,
    },
    .NotificationEndpoint           =
    {
      .Address                = CDC1_NOTIFICATION_EPADDR,
      .Size                   = CDC_NOTIFICATION_EPSIZE,
      .Banks                  = 1,
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
      .Banks                  = 1,
    },
    .DataOUTEndpoint                =
    {
      .Address                = CDC2_RX_EPADDR,
      .Size                   = CDC2_TXRX_EPSIZE,
      .Banks                  = 1,
    },
    .NotificationEndpoint           =
    {
      .Address                = CDC2_NOTIFICATION_EPADDR,
      .Size                   = CDC_NOTIFICATION_EPSIZE,
      .Banks                  = 1,
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

  //GlobalInterruptEnable();

  /* restart jennic and set to normal mode. XXX needs serial line ops */
  Serial_Config(1000000, 8, CDC_LINEENCODING_OneStopBit, CDC_PARITY_None);
  Jennic_Set_Mode(false);

  clock_prescale_set(clock_div_1);
}
void lufaLoop(void)
{
  {
  char buf[256];
  uint32_t m = millis();
  snprintf(buf,sizeof(buf),"millis: %u\n",m);
  CDC_Device_SendString(&VirtualSerial_CDC0_Interface, buf);
  }
    /* Jennic Mode and Baudrate */
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, TCNT0);
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, TCCR0A);
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, TCCR0B);
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, 0);
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, 0xFF);
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, 0xFF);
//CDC_Device_SendByte(&VirtualSerial_CDC0_Interface, 0xFF);
    if (jennic_reset_event)
    {
      
	CDC_Device_SendString(&VirtualSerial_CDC0_Interface, "jennic_reset_event set");
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

        /* reset USB connection */
        //USB_Detach();
        //_delay_ms(5000);
        //USB_Attach();
      }
      jennic_reset_event = false;
    }

    /* do house-keeping */
    USB_USBTask();
    Jennic_In_Task();
    CDC_In_Task();
    CDC_Device_USBTask(&VirtualSerial_CDC1_Interface);
    CDC_Device_USBTask(&VirtualSerial_CDC0_Interface);

    if ( ringbuf_elements(&USBtoUSART_Buffer) && !(UCSR1B & (1 << UDRIE1)) )
      UCSR1B |= (1 << UDRIE1);
  
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* enable clock division, run on 16MHz */
  //clock_prescale_set(clock_div_1);
  //clock_init();

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
  //sleep_disable();
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
  /* go to deep-sleep */
  //set_sleep_mode(SLEEP_MODE_STANDBY);
  //sleep_mode();
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
  bool ConfigSuccess = true;

  /* configure specific interfaces */
  ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC0_Interface);
  ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC1_Interface);

  /* if successful tell jennic */
  //if (ConfigSuccess)
  //{
  //  if (rndis)
  //  {
  //    /* activate RNDIS on jennic */
  //    Serial_SendByte(SLIP_END);
  //    Serial_SendByte('R');
  //    Serial_SendByte('N');
  //    Serial_SendByte('D');
  //    Serial_SendByte('I');
  //    Serial_SendByte('S');
  //    Serial_SendByte(SLIP_END);
  //  }
  //  else
  //  {
  //    /* stop SD access on jennic */
  //    Serial_SendByte(SLIP_END);
  //    Serial_SendByte('S');
  //    Serial_SendByte('D');
  //    Serial_SendByte(SLIP_END);

  //    SDManager_Init();
  //  }
  //}
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
 
  if(USB_ControlRequest.wIndex == 0)
  {
  	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC0_Interface);
  }
  else{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC1_Interface);
  }
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
  uint8_t ReceivedByte;

  if (ringbuf_elements(&USARTtoUSB_Buffer) >= ringbuf_size(&USARTtoUSB_Buffer) - 1 )
    return;

  ReceivedByte = UDR1;

  if (USB_DeviceState == DEVICE_STATE_Configured) {
    ringbuf_put(&USARTtoUSB_Buffer, ReceivedByte);
  }
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
	if(CDCInterfaceInfo->Config.ControlInterfaceNumber == 2)
        {
	/* fire a reset jennic event iff there is a high-low-high transition within
  	 * SWITCH_TIMEOUT ms on the dtr line */
  		// static clock_time_t previous_change = 0;
  		static bool currentDTRState = false;

		static unsigned long jennic_reset_time = 0;
  	/* dtr line changed? */
  		if ((CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != currentDTRState)
  		{
    			unsigned long delay = millis()-jennic_reset_time;
    			currentDTRState    = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);
    			jennic_reset_event = currentDTRState && (delay < 2500);
    			jennic_reset_time    = millis(); /* returns the usb frame clock in ms */
  		}
	}
	else if(millis() > 5000) // TODO wait 5000milis after USB Attach
	{
	// Reset Arduino for Programming
		if(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS == 1200)
                {
                        bool currentDTRState  = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);

                        if(currentDTRState == 0)
{
                                run_bootloader();
				
				//MCUCR = (1<<IVCE); // change reset vector to bootloader section
                                //MCUCR = (1<<IVSEL);
                                //wdt_enable(WDTO_120MS);
                        }
                        else
                        {
                                //wdt_disable();
                                wdt_reset();
                        }
                } // baudrate check

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
  _delay_ms(5);

  if (programming)
  {
    DDRC &= ~JEN_RESETN;
    _delay_ms(10);
    DDRB &= ~JEN_SPIMISO;
  }
  else
  {
    DDRB &= ~JEN_SPIMISO;
    _delay_ms(10);
    DDRC &= ~JEN_RESETN;
  }


}

void CDC_In_Task()
{
  /* Read bytes from the USB OUT endpoint and transmit to jennic if programming mode */
  if ( ringbuf_elements(&USBtoUSART_Buffer) < ringbuf_size(&USBtoUSART_Buffer)-2 ) {
    int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC1_Interface);
    if ( !(ReceivedByte < 0) )
      ringbuf_put(&USBtoUSART_Buffer, ReceivedByte);
  }
}

void Jennic_In_Task()
{ // XXX better buffering for CDC packets
  size_t i;

  Endpoint_SelectEndpoint(VirtualSerial_CDC1_Interface.Config.DataINEndpoint.Address);
  if ( !Endpoint_IsINReady() )
    return;

  /* Read bytes from the USART receive buffer and create ethernet frame or send with CDC */
  while (ringbuf_elements(&USARTtoUSB_Buffer))
  {
    uint8_t byte = ringbuf_get(&USARTtoUSB_Buffer);

    /* pass through in programming mode */
    if (jennic_in_programming_mode)
    {
      if (CDC_Device_SendByte(&VirtualSerial_CDC1_Interface, byte) != ENDPOINT_READYWAIT_NoError)
        return;
    }
  }
}
