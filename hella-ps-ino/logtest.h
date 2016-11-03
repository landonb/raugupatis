// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LOG_H__
#define __LOG_H__

#include <SoftwareSerial.h>

// Prototypes.
void testprint(HardwareSerial *rfid);

void testprint(HardwareSerial *rfid) {
	rfid->println("TEST!");
}

#endif // __LOG_H__

