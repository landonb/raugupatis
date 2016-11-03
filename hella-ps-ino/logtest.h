// vim:tw=0:ts=4:sw=4:noet:
#include <SoftwareSerial.h>

// Prototypes.
void testprint(HardwareSerial *rfid);

void testprint(HardwareSerial *rfid) {
	rfid->println("TEST!");
}

