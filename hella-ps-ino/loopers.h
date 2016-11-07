// Last Modified: 2016.11.06
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LOOPERS_H__
#define __LOOPERS_H__

#include "hella-ps.h"

void loopers_setup(void);
void loopers_loop(void);

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

#endif // __LOOPERS_H__

