// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "pins.h"
#include "rfid.h"
#include "state.h"

String Helladuino::get_state_name(void) {
	switch (this->state) {
		case STATE_BORED:
			return "Bored";
		case STATE_ENGAGED:
			return "Engaged";
		case STATE_POURING:
			return "Pouring";
		case STATE_STOLEN:
			return "Stolen";
		default:
			return "Unbevievable";
	}
}

void Helladuino::setup(void) {
	Serial.begin(9600);
	Serial.println("Start");

	this->upstream = &Serial;

	this->trace("Hello, Beer!");

	pins_setup();

	rfid_setup();
}

void Helladuino::loop(void) {
	boolean handled = false;

	this->trace("Helladuino::loop: Current state: " + this->get_state_name());

	pins_loop(this);

	uint8_t stealing = check_steal_button();
	if (stealing) {
		if (this->state != STATE_STOLEN) {
			rfid_reset();
			pins_transition(STATE_STOLEN);
			state_transition(STATE_STOLEN);
		}
		// else, being stolen and already in STATE_STOLEN; no-op.
		// Set handled to true so we skip other state checks.
		handled = true;
	}
	else {
		if (this->state == STATE_STOLEN) {
			rfid_reset();
			pins_transition(STATE_BORED);
			state_transition(STATE_BORED);
			handled = true;
		}
		// else, not being stolen and not stolen already,
		//       so check other states.
	}

	if (!handled) {
		rfid_loop();
	}
}

void Helladuino::trace(const String &s) {
	if (DEBUG) {
		this->upstream->println(s);
	}
}

void Helladuino::put_msg(const String &s) {
	if (!DEBUG) {
		this->upstream->println(s);
	}
}

void Helladuino::state_transition(HellaState new_state) {
	this->state = new_state;
	this->trace("Helladuino::state_transition: New state: " + this->get_state_name());
}

