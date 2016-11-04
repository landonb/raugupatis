// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "comm.h"
#include "state.h"

//boolean comm_authenticate(int rfid_tag[4]) {
boolean comm_authenticate(uint8_t ibutton_addr[8]) {
	boolean authenticated = false;

// FIXME: Mocked until wired....
	authenticated = true;
	return authenticated;
}

void comm_update_state(HellaState curr_state) {

// FIXME: This guy...

}

