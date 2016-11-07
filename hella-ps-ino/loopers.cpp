// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "loopers.h"

#include "comm.h"

// C++ /shim/ *noun* dog and pony show

Helladuino hellaps;

void loopers_setup()
{
	hellaps.setup();
}

// Loopers like to loop.
void loopers_loop()
{
	// 2016-11-04: [lb] learns Wiring
	//
	//              https://arduinohistory.github.io/
	//
	//             and I leave lots of breadcrumbs.
	//
	// Regarding interrupts:
	//
	//   noInterrupts();
	//   // critical, time-sensitive code here
	//   interrupts();
	//   // other code here
	//
	//   https://www.arduino.cc/en/Reference/Interrupts

	hellaps.loop();
}

void contract(const bool assertion, const char *file, const unsigned long line) {
	hellaps.comm->contract(assertion, file, line);
}

