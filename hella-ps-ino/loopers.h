// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LOOPERS_H__
#define __LOOPERS_H__

#include "pins.h"
#include "rfid.h"
#include "state.h"

// Devices we attach.
// - RFID scanner (2 digital pins)
// - Indicators (1 pin each):
//    Red
//    Green
//    Yellow
// - Switches
//    Red
//    Green
// - Solenoid/relay
// - Flow meter

void loopers_setup(void);
void loopers_loop(void);

Helladuino hellaps;

// The *real* setup(), called from the main *.ino's setup(),
// because [lb] is using the Arduino IDE and I don't want to
// have to copy-paste the main *.ino file to the IDE every
// time I edit it in Vim.
// UPDATE: I've since moved everything to a class, so shim-shim.
void loopers_setup()
{
//	pins_setup();
//	rfid_setup();
	hellaps.setup();
}

void loopers_loop()
{
	// Regarding interrupts:
	//
	//   noInterrupts();
	//   // critical, time-sensitive code here
	//   interrupts();
	//   // other code here
	//
	//   https://www.arduino.cc/en/Reference/Interrupts
//	pins_loop(&hellaps);
//	rfid_loop();
	hellaps.loop();
}

#endif // __LOOPERS_H__

