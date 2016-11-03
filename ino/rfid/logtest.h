// vim:tw=0:ts=4:sw=4:noet:
#include <SoftwareSerial.h>

void testprint(HardwareSerial *rfid) {
	rfid->println("TEST!");
}

