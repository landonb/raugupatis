// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "bluedot.h"
#include "comm.h"
#include "pins.h"
//#include "rfid.h"
#include "state.h"

String Helladuino::get_state_name(void) {
	switch (this->state) {
		case STATE_BORED:
			return "Bored";
		case STATE_BUZZ_OFF:
			return "Buff Off!";
		case STATE_ENGAGING:
			return "Engaging";			
		case STATE_DEGAGING:
			return "Disengaging";
		case STATE_ENGAGED:
			return "Engaged";
		case STATE_POURING:
			return "Pouring";
		case STATE_STOLEN:
			return "Stolen";
		default:
			// Unreachable.
			return "Unbevievable";
	}
}

void Helladuino::setup(void) {
	Serial.begin(9600);
	Serial.println("Start");
	this->upstream = &Serial;
	this->trace("Hello, Beer!");

	pins_setup();

	// 2016-11-03: We fried the Sparkfun RFID reader.
	//rfid_setup();
	// 2016-11-04: Could not get the Funduino RFID reader working.
	// Thankfully I had a 1 Wire blue dot and iButtons that work *easy*!
	bluedot_setup();

	pins_setup();
}

void Helladuino::loop(void) {
	boolean handled = false;

	//this->trace("Helladuino::loop: Current state: " + this->get_state_name());

	pins_loop(this);

	// There are only 2 interrupts on the Arduino, which are both is use,
	// so we poll for the state of the steal button.
	// NOTE: This means the user has to keep the steal button pressed
	//       to remain in this state!
	uint8_t stealing = check_steal_button();
	if (stealing) {
		if (this->state != STATE_STOLEN) {
			state_transition(STATE_STOLEN);
		}
		// else, being stolen and already in STATE_STOLEN; no-op.
		// Set handled to true so we skip other state checks.
		handled = true;
	}
	else {
		if (this->state == STATE_STOLEN) {
			state_transition(STATE_BORED);
			handled = true;
		}
		// else, not being stolen and not stolen already,
		//       so check other states.
	}

	if (handled) {
		//rfid_reset();
		bluedot_reset();
		// We transitioned from something to stolen, or from stolen
		// to bored, so ignore the action button, if it was pressed.
		this->action_desired = this->action_state;
	}
	else {
		if (this->state == STATE_BORED) {
			// See if a user swiped an RFID.
			//int rfid_tag[4];
			//boolean swiped = rfid_get_tag(rfid_tag);
			uint8_t ibutton_addr[8];
			String key_status;
			boolean swiped = bluedot_get_key_code(ibutton_addr, key_status);
			this->trace("loop: bluedot_get_key_code: swiped: " + (swiped ? String("yes") : String("no")));
			if (swiped) {
				//this->trace("loop: bluedot_get_key_code: ibutton_addr: " + ibutton_addr);
				this->trace("loop: bluedot_get_key_code: ibutton_addr: ");
				//this->trace(ibutton_addr);
				for (int i = 0; i < 8; i++) {
					this->trace(" index: " + (String)i + " / " + ibutton_addr[i]);
				}
				this->trace("loop: bluedot_get_key_code: key_status: " + key_status);
				this->trace("Helladuino::loop: ibutton_addr:");
				this->trace((String)ibutton_addr[0]);
				this->trace((String)ibutton_addr[1]);
				this->trace((String)ibutton_addr[2]);
				this->trace((String)ibutton_addr[3]);
				this->trace((String)ibutton_addr[4]);
				this->trace((String)ibutton_addr[5]);
				this->trace((String)ibutton_addr[6]);
				this->trace((String)ibutton_addr[7]);
				boolean authenticated = comm_authenticate(ibutton_addr, this);
				if (authenticated) {
					this->state_transition(STATE_ENGAGING);
				}
				else {
					this->state_transition(STATE_BUZZ_OFF);
				}
			}
			// else, nothing swiped, and we didn't detect the
			//       stolen button, so nothing to do this loop.
			//       MAYBE: Flash the lights funny and make little noises.
		}
		else {
			// In STATE_BUZZ_OFF,
			//    STATE_ENGAGED/STATE_ENGAGING/STATE_DEGAGING,
			// or STATE_POURING,
			// so ignore RFID.
			//rfid_reset();
			bluedot_reset();

			if (this->action_desired != this->action_state) {
				if ((this->state == STATE_ENGAGED)
					|| (this->state == STATE_DEGAGING)
				) {
					this->state_transition(STATE_POURING);
				}
				else if (this->state == STATE_POURING) {
					this->state_transition(STATE_ENGAGED);
				}
				else {
					// In STATE_ENGAGING or STATE_BUZZ_OFF.
					// We'll just ignore this....
				}
			}
			else {
				// User hasn't pressed action button since last loop.
				// If STATE_POURING, just stay in this state.
				// If in another state, check the timeout.
				int state_uptime = millis() - this->state_time_0;
				switch (this->state) {
					case STATE_BUZZ_OFF:
						if (state_uptime >= timeout_degaging) {
							this->state_transition(STATE_BORED);
						}
						// else, pins_loop handled the lights for this state.
						break;
					case STATE_ENGAGING:
						if (state_uptime >= timeout_engaging) {
							this->state_transition(STATE_ENGAGED);
						}
						// else, pins_loop handled the lights for this state.
						break;
					case STATE_ENGAGED:
						if (state_uptime >= timeout_engaged_de_auth) {
							this->state_transition(STATE_BORED);
						}
						else if (state_uptime >= timeout_engaged_warning) {
							this->state_transition(STATE_DEGAGING);
	// FIXME/LATER: If beer starts flowing, resume STATE_ENGAGED.
						}
						// else, less time than the timeout, stay engaged.
						break;
					case STATE_DEGAGING:
						if (state_uptime >= timeout_degaging) {
							this->state_transition(STATE_BORED);
						}
						// else, pins_loop handled the lights for this state.
						break;
					case STATE_POURING:
						// No-op. Stay in state.
						break;
					case STATE_STOLEN:
						// No-op. Stay in state.
						break;
					default:
						// Unreachable.
						break;
				}
			}
		}
	}
	//this->trace("Helladuino::loop: Final state: " + this->get_state_name());
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
	else {
		// I love Arduino's String hack. [lb]
		this->upstream->println("MOCK/put_msg: " + s);
	}
}

void Helladuino::put_byte(uint8_t byte) {
	if (!DEBUG) {
		this->upstream->write(byte);
	}
	else {
		// I love Arduino's String hack. [lb]
		this->upstream->println("MOCK/put_byte: 0x" + byte);
	}
}

boolean Helladuino::get_byte(uint8_t& incoming_byte) {
	// https://www.arduino.cc/en/Reference/Serial
	// Serial.readBytes() blocks until the timeout.
	// Serial.setTimeout(time) defaults to 1000 (milliseconds).
	// Serial.real() doesn't block as far as I can tell.
	//
	boolean got_byte = false;
	// The receive buffer holds 64 bytes until we read it.
	//  https://www.arduino.cc/en/Serial/Available
	if (this->upstream->available() == 0) {
		// Should we delay?
		delay(500);
	}
	if (this->upstream->available() > 0) {
		if (!DEBUG) {
			incoming_byte = this->upstream->read();
		}
		else {
			incoming_byte = 0x00;
			this->upstream->println("MOCK/get_byte: 0xNN");
		}
		got_byte = true;
	}
	return got_byte;
}

void Helladuino::state_transition(HellaState new_state) {
	this->state = new_state;
	this->state_time_0 = millis();
	pins_transition(new_state);
	comm_update_state(new_state);
	this->trace(
		"Helladuino::state_transition: New state: "
		+ this->get_state_name()
		+ " at "
		+ this->state_time_0
		+ " millis"
	);
}

