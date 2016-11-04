// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "comm.h"
#include "pins.h"
#include "rfid.h"
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
		rfid_reset();
		// We transitioned from something to stolen, or from stolen
		// to bored, so ignore the action button, if it was pressed.
		this->action_desired = this->action_state;
	}
	else {
		if (this->state == STATE_BORED) {
			// See if a user swiped an RFID.
			int rfid_tag[4];
			boolean swiped = rfid_get_tag(rfid_tag);
			if (swiped) {
				this->trace(
					"Helladuino::loop: rfid_tag: "
					+ rfid_tag[0]
					+ rfid_tag[1]
					+ rfid_tag[2]
					+ rfid_tag[3]
				);
				boolean authenticated = comm_authenticate(rfid_tag);
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
			rfid_reset();

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
					case STATE_ENGAGED:
						if (state_uptime >= timeout_engaged_de_auth) {
							this->state_transition(STATE_BORED);
						}
						else if (state_uptime >= timeout_engaged_warning) {
							this->state_transition(STATE_DEGAGING);
							// FIXME: If beer starts flowing, resume STATE_ENGAGED.
						}
						// else, less time than the timeout, stay engaged.
						break;
					case STATE_POURING:
						// No-op. Stay in state.
						break;
					case STATE_ENGAGING:
						if (state_uptime >= timeout_engaging) {
							this->state_transition(STATE_ENGAGED);
						}
						// else, pins_loop handled the lights for this state.
						break;
					case STATE_BUZZ_OFF:
						if (state_uptime >= timeout_degaging) {
							this->state_transition(STATE_BORED);
						}
						// else, pins_loop handled the lights for this state.
						break;
					default:
						// Unreachable.
						// FIXME: contract.Contract(false);
						break;
				}
			}
		}
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

