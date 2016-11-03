// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LOG_H__
#define __LOG_H__

#include <SoftwareSerial.h>

// Prototypes.
void testprint(HardwareSerial *upstream);

void testprint(HardwareSerial *upstream) {
	// https://www.arduino.cc/en/Serial/Println
	upstream->println("TEST!");
}

#endif // __LOG_H__

