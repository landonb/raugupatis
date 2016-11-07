// Last Modified: 2016.11.06
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

//#include <stdio.h>
//#include <stdarg.h>

#include "Arduino.h"

#include "hella-ps.h"

#include "bluedot.h"
#include "comm.h"
#include "pins.h"
//#include "rfid.h"
#include "state.h"

void Helladuino::setup(void) {

	this->comm = new CommUpstream();
	this->pins = new InputsOutputs();
	this->bluedot = new BlueDot();
	this->state = new StateMachine();

	// Get the USB connection up first so we can send trace messages.
	this->comm.setup();
	this->trace("Hello, Beer!");

	// Get the pins setup next so we can light up the user display.
	this->pins.setup();

	// 2016-11-03: We fried the Sparkfun RFID reader.
	//rfid_setup();
	// 2016-11-04: Could not get the Funduino RFID reader working, either.
	// Thankfully I had a 1 Wire blue dot and iButtons that work *easy*!
	this->bluedot.setup();

	// Enter the STATE_BORED state.
	this->state.setup(this);
}

void Helladuino::loop(void) {
	this->state.loop();
}

void Helladuino::trace(const char *msg, ...) {
	va_list argp;
	va_start(argp, msg);
	this->comm.vtrace(msg, argp);
	va_end(argp);
}

