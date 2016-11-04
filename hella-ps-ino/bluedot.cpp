// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "bluedot.h"
#include "OneWire.h"

OneWire ds(12);

void bluedot_setup(void) {
	// no-op
}

boolean bluedot_get_key_code(uint8_t ibutton_addr[8], String& key_status) {
	boolean got_key_code = false;

	key_status = "";

	if (!ds.search(ibutton_addr)) {
		key_status = "nothing found";
		ds.reset_search();
	}
	else if (OneWire::crc8(ibutton_addr, 7) != ibutton_addr[7]) {
		key_status = "CRC invalid";
	}
	else if (ibutton_addr[0] != 0x01) {
		key_status = "not DS1990A";
	}
	else {
		// Bueno!
		got_key_code = true;
		key_status = "ok";
	}

	ds.reset();

	return got_key_code;
}

void bluedot_reset() {
	// [lb] just guessing.
	ds.reset_search();
	ds.reset();
}

