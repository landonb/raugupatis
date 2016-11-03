// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LOOPERS_H__
#define __LOOPERS_H__

#include "logtest.h"
#include "lights.h"
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

HellaPsState hellaps;

// The *real* setup(), called from the main *.ino's setup(),
// because [lb] is using the Arduino IDE and I don't want to
// have to copy-paste the main *.ino file to the IDE every
// time I edit it in Vim.
void loopers_setup()
{
	Serial.begin(9600);
	Serial.println("Start");

	testprint(&Serial);

	rfid_setup();

	lights_setup();
}

void loopers_loop()
{
	rfid_loop();

	lights_loop(&hellaps);
}

#endif // __LOOPERS_H__

