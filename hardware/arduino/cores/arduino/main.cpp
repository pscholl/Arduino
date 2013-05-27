#include <Arduino.h>
#include <DualVirtualSerial.h>

int main(void)
{
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
