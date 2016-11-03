// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "pins.h"
#include "rfid.h"
#include "state.h"

void Helladuino::setup(void) {
	Serial.begin(9600);
	Serial.println("Start");

	this->upstream = &Serial;

	this->trace("Hello, Beer!");

}

void Helladuino::loop(void) {




//	uint8_t stealing = check_steal_button();

/*
	if ((stealing) && (hellaps->state != STATE_STOLEN)) {
		hellaps->go_stolen();
	}

	switch(hellaps->state) {
		case STATE_BORED:
			if (stealing) {
				
			}
	}

	if (stealing && (STATE_STOLEN

	if (hellaps->state == STATE_BORED) {
		if (stealing) {
		}
	}
*/


	this->trace("YESSSSSSSSSSS");
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

void Helladuino::go_stolen(void) {
	this->state = STATE_STOLEN;
}

