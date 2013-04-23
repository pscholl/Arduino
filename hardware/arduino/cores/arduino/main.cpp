#include <Arduino.h>

#include <DualVirtualSerial.h>

int main(void)
{
	/* Arduino  init and setup */
	lufaInit();
	init();

	setup();
	
    
	for (;;) {
		lufaLoop();
		loop();
	}
        
	return 0;
}
