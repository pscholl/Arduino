#include <Arduino.h>
#include <DualVirtualSerial.h>

int main(void)
{

	DDRF |= (_BV(4));
	PORTF &= ~(_BV(4));
	/* Arduino  init and setup */
	init();
	lufaInit();

	setup();
	
    
	for (;;) {
		lufaLoop();
		loop();
	}
        
	return 0;
}
