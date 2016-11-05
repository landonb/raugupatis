// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "comm.h"
#include "state.h"

//boolean comm_authenticate(int rfid_tag[4]) {
boolean comm_authenticate(uint8_t ibutton_addr[8], Helladuino *hellaps) {
	boolean authenticated = false;

	uint8_t checksum = 0;

	hellaps->trace("comm_authenticate: Sending request...");

	// HEADER 0x00.
	hellaps->put_byte((uint8_t)0);
	// COMMAND Authenticate 0x01.
	hellaps->put_byte((uint8_t)1);
	checksum += 1;

	// TOKEN/KEY.
	for (int i = 0; i < 8; i++) {
		hellaps->put_byte((uint8_t)ibutton_addr[i]);
		checksum += ibutton_addr[i];
	}

	// CHECKSUM.
	hellaps->put_byte((uint8_t)checksum);
	// FOOTER 0xFF 0xFF.
	hellaps->put_byte((uint8_t)0xFF);
	hellaps->put_byte((uint8_t)0xFF);

	// And then what? Block on read?
	hellaps->trace("comm_authenticate: Awaiting response...");

	boolean got_byte;
	uint8_t incoming_byte;
	got_byte = hellaps->get_byte(incoming_byte);

	if (got_byte) {
		hellaps->trace("comm_authenticate: incoming_byte: " + incoming_byte);
		//if (incoming_byte == 'T') {
		if (incoming_byte == 1) {
			authenticated = true;
		}
		// else, (incoming_byte == 'F')
	}
	else {
		hellaps->trace("comm_authenticate: No response");
	}

	return authenticated;
}

void comm_update_state(HellaState curr_state) {

// FIXME: This guy...

}

