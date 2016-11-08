// Last Modified: 2016.11.08
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "loopers.h"

#include "comm.h"

// C++ /shim/ *noun* dog and pony show

// Hrmm. Dynamically allocating this object saves a tiny but of SRAM.
//   Helladuino hellaps;
//     Global variables use 1,409 bytes (68%) of dynamic memory, leaving 639 bytes for local variables. Maximum is 2,048 bytes.
//   Helladuino *hellaps;
//     Global variables use 1,367 bytes (66%) of dynamic memory, leaving 681 bytes for local variables. Maximum is 2,048 bytes.
Helladuino *hellaps;

void loopers_setup()
{
	hellaps = new Helladuino();
	//hellaps.setup();
	hellaps->setup();
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

	//hellaps.loop();
	hellaps->loop();
}

void contract(const bool assertion, const char *file, const unsigned long line) {
	//hellaps.comm->contract(assertion, file, line);
	hellaps->comm->contract(assertion, file, line);
}

void contract(const bool assertion, const unsigned long file, const unsigned long line) {
	hellaps->comm->contract(assertion, file, line);
}

