// Last Modified: 2016.11.06
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "state.h"

#include "bluedot.h"
#include "comm.h"
#include "hella-ps.h"
#include "pins.h"
//#include "rfid.h"

const char *StateMachine::get_state_name(void) {
	// As far as I'm aware, these char*s are allocated by the compiler
	// and are not allocated/released/managed at runtime.
	switch (this->state) {
		case STATE_NONE:
			return "WHAH?!: NONE";
		case STATE_BORED:
			return "Bored";
		case STATE_BUZZ_OFF:
			return "Buff Off!";
		case STATE_ENGAGING:
			return "Engaging";			
		case STATE_ENGAGED:
			return "Engaged";
		case STATE_PATIENCE:
			return "Paitence";
		case STATE_POURING:
			return "Pouring";
		case STATE_GULPING:
			return "Gulping";
		case STATE_DEGAGING:
			return "Disengaging";
		case STATE_EIGHTYSIX:
			return "Eighty sixxed";
		case STATE_STEALING:
			return "Stealing";
		case STATE_STOLEN:
			return "Stolen";
		case STATE_SKULKING:
			return "Skulking";
		default:
			// Unreachable.
			return "Unbevievable";
	}
}

void StateMachine::setup(Helladuino *hellaps) {
	this->comm = hellaps->comm;
	this->pins = hellaps->pins;
	this->bluedot = hellaps->bluedot;

	this->transition(STATE_BORED);
}

void StateMachine::loop(void) {
	this->check_stolen_state();
	this->check_beerme_state();
	this->check_atoken_state();
	this->manage_current_state();
}

void StateMachine::check_stolen_state(void) {
	// There are only 2 interrupts on the Arduino (without an additional Shield),
	// which are both in use (Green button and flowmeter), so we poll for the state
	// of the steal button.
	// NOTE: This means user has to keep the steal button pressed to remain in this state!
	uint8_t steal_button_pressed = this->pins.check_steal_button();
	if (steal_button_pressed) {
		if (this->state != STATE_EIGHTYSIX) {
			// Except for the Bored state and Stolen states, any
			// Red Button press routes through the EightySix state
			// back to the Bored state. In the Bored state, a Red
			// Button press proceeds into the Stolen states.
			if (false
				|| (this->state == STATE_ENGAGING)
				|| (this->state == STATE_ENGAGED)
				|| (this->state == STATE_PATIENCE)
				|| (this->state == STATE_POURING)
				|| (this->state == STATE_GULPING)
				|| (this->state == STATE_DEGAGING)
			) {
				// An authenticated user is allowed the privilege
				// of immediately terminating their session.
				this->transition(STATE_EIGHTYSIX);
			}
			else if (false
				|| (this->state == STATE_BUZZ_OFF)
			) {
				// These state(s) already lead to STATE_BORED, so do nothing;
				// keep user in suspense.
			}
			else if (true
				&& (this->state != STATE_STEALING)
				&& (this->state != STATE_STOLEN)
				&& (this->state != STATE_SKULKING)
			) {
				// I.e.,
				//  STATE_NONE (impossible)
				//  STATE_BORED
				// One can only start stealing from the STATE_BORED state.
				this->transition(STATE_STEALING);
			}
			// else,
			//       being stolen (STATE_STEALING)
			//  or already stolen (STATE_STOLEN)
			//  or being unstolen (STATE_SKULKING).
		}
		// else, STATE_EIGHTYSIX, gotta wait it out.
	}
	else {
		// User is not pressing Red Stealing Button. Check if they were previously.
		if (false
			|| (this->state == STATE_STOLEN)
			|| (this->state == STATE_STEALING)
		) {
			this->transition(STATE_SKULKING);
		}
		// else, already Skulking, or in a non-stolen state.
	}
}

void StateMachine::check_beerme_state(void) {
	// The Green button's ISR, on_action_button_isr, captures buttons clicks.
	// Here we check if the button was pressed (an odd number of times) since
	// last loop. We only care about action presses when the user is
	// authenticated, and if we're not in an authenticated transitional state.

	bool latest_beerme = this->pins.get_beerme_state();

	bool button_pressed = (latest_beerme != this->beerme_state);

	if (button_pressed) {
		if (false
			// Skipping: STATE_NONE
			//// Skipping: STATE_BORED
			|| (this->state == STATE_BORED)
			// Skipping: STATE_BUZZ_OFF
			// Skipping: STATE_ENGAGING
			|| (this->state == STATE_ENGAGED)
			// Skipping: STATE_PATIENCE
			|| (this->state == STATE_POURING)
			// Skipping: STATE_GULPING
			|| (this->state == STATE_DEGAGING)
			// Skipping: STATE_EIGHTYSIX
			// Skipping: STATE_STEALING
			// Skipping: STATE_STOLEN
			// Skipping: STATE_SKULKING
		) {
			char* t_or_f = latest_beerme ? "true" : "false";
			this->comm->trace("check_beerme_state: latest_beerme: %s", t_or_f);

			if (latest_beerme) {
				if (this->state == STATE_BORED) {
					// Punishment!
					this->transition(STATE_BUZZ_OFF);
				}
				else if (false
					|| (this->state == STATE_ENGAGED)
					|| (this->state == STATE_DEGAGING)
				) {
					this->transition(STATE_ENGAGING);
				}
				else {
					// STATE_POURING. And latest_beerme? Shouldn't happen.
					contract(false);
				}
			}
			else {
				// !lastest_beerme, so user disengaging.
				if (this->state == STATE_POURING) {
					this->transition(STATE_GULPING);
				}
				else {
					// STATE_ENGAGED or STATE_DEGAGING. And !latest_beerme? Shouldn't happen.
					contract(false);
				}
			}
		}
	}
	else {
		if (latest_beerme != this->beerme_state) {
			this->comm->trace("check_beerme_state: ignoring button press")
			if (this->state == STATE_BORED) {
				// Why not.
				this->transition(STATE_BUZZ_OFF);
			}
		}
FIXME: maintain state sometimes.....
this->beerme_state = false;
this->desired_state = false;
		this->pins.set_beerme_state(false);
	}
}

void StateMachine::check_atoken_state(void) {
	// If we're in a state where we don't care about the authentication
	// event, don't waste time looking for it; keep animating.
	if (false
		|| (this->state == STATE_BORED)
		// 2016-11-06: Maybe when engaged, to speed the beer line along,
		// we can let auth'ed users overlap (with a song 'n dance show).
		|| (this->state == STATE_ENGAGED)
		// We could let a new user log on immediately after Red Button is
		// depressed but let's annoy the new user so that the thief is punished.
		//|| (this->state == STATE_SKULKING)
	) {
		// Check if a user swiped an RFID/iButton/AuthWand.
		uint8_t ibutton_addr[8];
		Bluedot_Key_Status key_status = this->bluedot.get_key_code(ibutton_addr);

		// FIXME: Probably comment this out unless swiped, else, too many prints.
		this->comm->trace(
			"check_atoken_state: this->bluedot.get_key_code: key_status: %s / ibutton_addr: %.*s"
			this->bluedot.get_key_status_name(key_status),
			8, ibutton_addr,
		);

		if (key_status == BLUEDOT_KEY_STATUS_VALID) {
			this->comm->trace("check_atoken_state: this->bluedot.get_key_code: ibutton_addr:");
			for (int i = 0; i < 8; i++) {
				this->comm->trace(" index: %s / 0x%x", i, ibutton_addr[i]);
			}

			bool authenticated = this->comm.authenticate(ibutton_addr, this);
			if (authenticated) {
				this->transition(STATE_ENGAGING);
			}
			else {
				this->transition(STATE_BUZZ_OFF);
			}
		}
	}
}

void StateMachine::manage_current_state(void) {


		// Check if user pressed Big Green Button.
		// NOTE: See: check_beerme_state. We already check the state,
		//       so if these differ, do as told.
FIXME: desired_state
		if (this->desired_state != this->beerme_state) {
			if ((this->state == STATE_ENGAGED)
				|| (this->state == STATE_DEGAGING)
			) {
				this->transition(STATE_POURING);
			}
			else if (this->state == STATE_POURING) {
				this->transition(STATE_ENGAGED);
			}
			else {
				// In STATE_ENGAGING or STATE_BUZZ_OFF.
				// We'll just ignore this....
				// FIXME: Do something fun with the lights.
			}
		}
		else {
			// User hasn't pressed action button since last loop.
			// If STATE_POURING, just stay in this state.
			//    FIXME: We should check flowmeter and timeout if not pouring beer.
			// If in another state, check the timeout.
			int state_uptime = millis() - this->state_time_0;
			switch (this->state) {
				case STATE_BUZZ_OFF:
					// An animation state. Resume being bored when finished animating.
					if (state_uptime >= timeouts.degaging) {
						this->transition(STATE_BORED);
					}
					// else, this->pins.animate will handle the lights for this state.
					break;
				case STATE_ENGAGING:
					// An animation state. Become engaged when finished animating.
					if (state_uptime >= timeouts.engaging) {
						this->transition(STATE_ENGAGED);
					}
					// else, this->pins.animate will handle the lights for this state.
					break;
				case STATE_DEGAGING:
					// An animation state. Go bored when finished animating. However,
					// unlike other animation states, user can recover from this one.
					if (state_uptime >= timeouts.degaging) {
						this->transition(STATE_BORED);
					}
					// else, this->pins.animate will handle the lights for this state.
					break;
				case STATE_STEALING:
					// Unreachable/already handled.
					contract(false);
					break;
					// An animation state. Become stolen when finished animating.
					if (state_uptime >= timeouts.stealing) {
						this->transition(STATE_STOLEN);
					}
					// else, this->pins.animate will handle the lights for this state.
					break;
				case STATE_SKULKING:
					// An animation state. Become bored once done skulking.
					if (state_uptime >= timeouts.skulking) {
						this->transition(STATE_BORED);
					}
					// else, this->pins.animate will handle the lights for this state.
					break;
				case STATE_ENGAGED:
					// The engaged state. User can enabled beer with Green button.
					// If user doesn't do anything, we timeout.
					if (state_uptime >= timeouts.engaged_degaging) {
						this->transition(STATE_DEGAGING);
					}
					// else, less time than the timeout, stay engaged.
					break;
				case STATE_POURING:
				case STATE_STOLEN:
					this->loop_pouring_or_stolen();
					break;
				default:
					// Unreachable.
					break;
			}
		}
	}

	this->pins.animate(this->state, this->state_time_0);

	//this->comm->trace("StateMachine::loop: Final state: %s", this->get_state_name());
}

void StateMachine::loop_pouring_or_stolen() {

	unsigned long pour_blip_n = this->pins.get_flowmeter_count();
	unsigned long pour_time_n = millis();

	unsigned long pour_time_elapsed = pour_time_n - this->pour_time_0;

	if (pour_blip_n == this->last_blip) {
		// The beer has not moved! What is wrong with people?
		unsigned long void_time_elapsed = pour_time_n - this->last_time;
		if (void_time_elapsed > timeouts.pouring_idle) {
		}
	}
	else {
		// The beer is flowing. Update our markers to keep timeouts at bay.
		this->last_blip = pour_blip_n;
		this->last_time = pour_time_n;
	}

	// Update the pi every once in a while (but not every loop()).
	if () {
	}

	if (
						
						// No-op. Stay in state.
// FIXME/LATER: If beer starts flowing, resume STATE_ENGAGED. -- RESET TIMEOUT
// get_flowmeter_count -- send to Pi
this->comm.update_state(new_state, state_duration);
						break;
						// No-op. Stay in state.
// get_flowmeter_count -- send to Pi
	// FIXME/LATER: If beer starts flowing, resume STATE_ENGAGED.

}

void StateMachine::transition(HellaState new_state) {


FIXME: WHat?
unsigned long state_duration = millis() - this->state_time_0;


	this->state = new_state;
	this->state_time_0 = millis();
	this->pins.transition(new_state);

	if ((new_state == STATE_POURING) || (new_state == STATE_STOLEN)) {
		this->pour_blip_0 = this->pins.get_flowmeter_count;
		this->pour_time_0 = millis();
		this->last_blip = this->pour_blip_0;
		this->last_time = this->pour_time_0;
	}

	this->pins.reset_beerme_state(new_state);

	// Whenever transitioning states clear the authentication device buffer
	// so we don't have to worry about it at other times (since the hardware
	// might be slow to clear, we don't do it during loop()).
	//rfid_reset();
	this->bluedot.reset();

	this->comm->trace(
		"StateMachine::transition: New state: %s at %d",
		this->get_state_name(), 
		this->state_time_0,
	);
}

