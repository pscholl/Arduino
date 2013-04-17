#include <Arduino.h>

#include <DualVirtualSerial.h>

int main(void)
{
	/* Arduino  init and setup */
	init();
	setup();
	
	/* LUFA Setup */
	SetupHardware();
	// LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();
    
	for (;;) {
		CDC1_Task();
		CDC2_Task();
		USB_USBTask();
		
		loop();
	}
        
	return 0;
}
