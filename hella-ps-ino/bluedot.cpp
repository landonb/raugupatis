// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "bluedot.h"

#include "OneWire.h"

// MAGIC_NUMBER: 12 is the pin number.
const uint8_t onewire_pin = 12;
OneWire ds(onewire_pin);

void BlueDot::setup() {
	// no-op
}

void BlueDot::reset() {
	// [lb] just guessing.
	ds.reset_search();
	ds.reset();
}

Bluedot_Key_Status BlueDot::get_key_code(uint8_t ibutton_addr[IBUTTON_LEN]) {
	Bluedot_Key_Status key_status = BLUEDOT_KEY_STATUS_UNKNOWN;

	if (!ds.search(ibutton_addr)) {
		key_status = BLUEDOT_KEY_STATUS_NOTHING_FOUND;
		ds.reset_search();
	}
	else if (OneWire::crc8(ibutton_addr, 7) != ibutton_addr[7]) {
		key_status = BLUEDOT_KEY_STATUS_CRC_INVALID;
	}
	else if (ibutton_addr[0] != 0x01) {
		key_status = BLUEDOT_KEY_STATUS_NOT_DS1990A;
	}
	else {
		// Bueno!
		key_status = BLUEDOT_KEY_STATUS_VALID;
	}

	ds.reset();

	return key_status;
}

// FIXME: Move to the Pi.
//        BECAUSE: pi should do stringification
/*
const char* BlueDot::get_key_status_name(Bluedot_Key_Status key_status) {
	switch (key_status) {
		case BLUEDOT_KEY_STATUS_UNKNOWN:
			return PSTR("ERROR: No such status: Unknown");
		case BLUEDOT_KEY_STATUS_VALID:
			return PSTR("Valid");
		case BLUEDOT_KEY_STATUS_NOTHING_FOUND:
			return PSTR("Nothing found");
		case BLUEDOT_KEY_STATUS_CRC_INVALID:
			return PSTR("CRC Invalid");
		case BLUEDOT_KEY_STATUS_NOT_DS1990A:
			return PSTR("Not DS1990A");
		case _BLUEDOT_KEY_STATUS_COUNT:
		default:
			return PSTR("ERROR: No such status: int > count");
	}
}
*/

