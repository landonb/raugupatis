// Last Modified: 2016.11.07
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

// NOTE: Use PROGMEM to avoid using precious dynamic memory (SRAM) for strings
//       and other global variables. Put them in flash (program) memory instead.
//   https://www.arduino.cc/en/Tutorial/Memory
//   https://www.arduino.cc/en/Reference/PROGMEM
//   - How come I learned about the F()/PSTR() macros not from Arduino
//     but from a blog?
//       http://www.gammon.com.au/progmem
// 2016-11-07: I added PROGMEM after having weird Serial errors.
//             I added a trace("state %s", get_state_name()) and
//             then the serial line was littered with random
//             bytes here and there (so-called rectangular tofu,
//             or black dots with question marks in them). Oddly,
//             even the first message ("Hello, Beer!") had this
//             problem, though the trace line I added was way later
//             in the execution. I gurgled "arduino serial weird characters"
//             and was pointed in the direction of taming the SRAM
//             by avoiding having too many globals (which reduces
//             the amount of room for local variables, and also might
//             mean that the stack and the heap end up overlapping maybe,
//             though I'm surprised I only saw weird characters and didn't
//             see the app crash).
// 2016-11-07: Using PSTR() macro shrunk global memory from 1,341 bytes to 641!

void Helladuino::setup(void) {

	this->comm = new CommUpstream();
	this->pins = new InputsOutputs();
	this->bluedot = new BlueDot();
	this->state = new StateMachine();

	// Get the USB connection up first so we can send trace messages.
	this->comm->setup();
	// WHATEVER: Changing this string to PROGMEM doesn't change global
	//           memory usage like I expected, e.g.,
	//              const char* const hello_msg PROGMEM = "Hello, Beer!";
	//              this->trace(hello_msg);
	//           But doing it inline with the PSTR macro works, where:
	//              #define PSTR(s) ((const PROGMEM char *)(s))
	//           Hrmmm.
	this->trace(PSTR("Hello, Beer!"));

	// Get the pins setup next so we can light up the user display.
	this->pins->setup();

	// 2016-11-03: We fried the Sparkfun RFID reader.
	//rfid_setup();
	// 2016-11-04: Could not get the Funduino RFID reader working, either.
	// Thankfully I had a 1 Wire blue dot and iButtons that work *easy*!
	this->bluedot->setup();

	// Enter the STATE_BORED state.
	this->state->setup(this);
}

void Helladuino::loop(void) {
	this->state->loop();
}

void Helladuino::trace(const char *msg, ...) {
	va_list argp;
	va_start(argp, msg);
	this->comm->vtrace(msg, argp);
	va_end(argp);
}

