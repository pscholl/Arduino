Arduino JNode Port
==================

This Arduino Fork ports the [Arduino][1] Environment to the [JNode][2].
Therefore the adapted Firmware provides two virtual Serial Interfaces via
the [LUFA][3] USB Library.
The first as an usual Arduino Serial Connection as known from the Arduino Leonardo.
The second is used to programm the Jennic.

For wireless communiction the Arduino EthernetClient and EthernetServer classes 
will be implemented, using the jennic for 6LoWPAN over ZigBee.

[1]: http://www.arduino.cc/ "Arduino"
[2]: https://github.com/teco-kit/Jennisense/wiki/Hardware#jnode---small-footprint-and-plenty-of-sensors
[3]: http://www.fourwalledcubicle.com/LUFA.php "LUFA"

Installation
============

For linux an updated build is available here [1], additionally the dfu-programmer [2] needs to be installed.

[1]: https://github.com/pscholl/Arduino/raw/master/build/linux/arduino-0104-linux.tgz

CREDITS
-------
The Arduino team is composed of Massimo Banzi, David Cuartielles, Tom Igoe,
Gianluca Martino, Daniela Antonietti, and David A. Mellis.
Arduino is an open source project, supported by many.

Arduino uses the GNU avr-gcc toolchain, avrdude, avr-libc, and code from
Processing and Wiring.

The LUFA Library is written by Dean Camera.

Icon and about image designed by ToDo: http://www.todo.to.it/

