// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __STATE_H__
#define __STATE_H__

// FIXME: Switch DEBUG to false when deploying to production.
//const DEBUG bool = false;
const boolean DEBUG = true;

const int timeout_engaging = 666;
const int timeout_degaging = 3210;

const int timeout_engaged_warning = 5000;
// FIXME: Hahaha, this might be too short to pour a beer.
//        Specifically, while user is pouring beer, this is no real
//         interaction with the system.
//        FIXME: When flowmeter is added, detecting flow should reset state_time_0.
//               
const int timeout_engaged_de_auth = 10000;


enum HellaState {
	// An illegal state, just so unset/0/nil/null/NULL/None isn't valid.
	STATE_NONE,
	// The idle state is the "bored" state.
	STATE_BORED,
	// When the used swipes an RFID, they'll transition to an
	// intermediate state so we can do a light and sound show.
	STATE_BUZZ_OFF,
	STATE_ENGAGING,
	// If the user swiped an RFID, was authenticated, and
	// transitioned to through the light and sound state,
	// they'll transition to the engaged state -- we're ready!
	STATE_ENGAGED,
	// For timing out on inactivity.
	STATE_DEGAGING,
	// Once engaged, the user can transition to and from the
	// pouring state (basically open/close the solenoid).
	STATE_POURING,
	// The special stolen state bypasses all other states.
	// I know, right!
	STATE_STOLEN,
};

// MAYBE: Use this, or not, once we implement the Pi side.
enum TokenStatus {
	TOKEN_UNSET,
	TOKEN_ACCEPTED,
	TOKEN_REJECTED,
};

//enum ActionDesired {
//	ACTION_NONE,
//	ACTION_POUR,
//	ACTION_STOP,
//};

class Helladuino {
public:

	HellaState state = STATE_BORED;

	unsigned long state_time_0 = 0;

	String user_token = "";
	TokenStatus token_status = TOKEN_UNSET;

	boolean action_state = false;
	// pins.cpp will set action_desired from its ISR.
	boolean action_desired = false;

	boolean steal_state = false;

	// During testing, USB connection used for dumping 
	HardwareSerial *upstream = NULL;

	// Interface.
	void setup(void);
	void loop(void);
	//
	String get_state_name(void);
	void trace(const String &s);
	void put_msg(const String &s);
	// State changes.
	void state_transition(HellaState new_state);

};

#endif // __STATE_H__

