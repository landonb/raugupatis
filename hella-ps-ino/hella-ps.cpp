// Last Modified: 2016.11.08
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
// 2016-11-07: Seriously, just adding too many string constants (like, as
//             function parameters), even when global memory is under 50%,
//             can cause the Arduino to seemingly randomly reboot.

void Helladuino::setup(void) {

	this->comm = new CommUpstream();
	this->pins = new InputsOutputs();
	this->bluedot = new BlueDot();
	this->state = new StateMachine();

	// Get the USB connection up first so we can send trace messages.
	this->comm->setup();

	// 2016-11-08: BEWARE: If you pass PSTRs around and don't treat them
	// specially, you could get weird program behavior that at least can
	// manifest itself as gargled or garbage in the serial output stream.
	//
	// I.e., introduce small changes in case you screw up memory, because
	// it's not always obvious what's happening or what caused it.
	//
	// For instance, on 2016-11-07, I had some code that worked as expected:
	//
	//   this->trace_P(PSTR("Hello, Beer! %s"), "YASSSS");
	//   this->trace_P0(PSTR("Hello, Beer!"));
	//
	//   Serial ready
	//   Hello, Beer! YASSSS
	//   Hello, Beer!
	//   
	//
	// But then I removed the first trace_P and all hell broke loose:
	//
	//   this->trace_P0(PSTR("Hello, Beer! 3"));
	//
	//   Serial ready
	//   IMQHello, Beer!
	//
	//   and the same dumb "I M Q " preceded every subsequent message!
	//
	// Though it's not always a weird prefix. Sometimes you'll see
	// crud mixed in with messages, or messages being repeated, or
	// partially repeated, etc.
	this->trace_P(PSTR("Hello, %s!"), "Beer");

	// Get the pins setup next so we can light up the user display.
	this->pins->setup(this);

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

void Helladuino::trace(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	this->comm->vtrace(fmt, argp);
	va_end(argp);
}

void Helladuino::trace_P(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	this->comm->vtrace_P(fmt, argp);
	va_end(argp);
}

void Helladuino::trace_P0(const char *fmt) {
	this->comm->trace_P0(fmt);
}

void Helladuino::put_msg(const char *msg) {
	this->comm->put_msg(msg);
}

