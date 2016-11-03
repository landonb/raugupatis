// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __STATE_H__
#define __STATE_H__

// FIXME: Switch DEBUG to false when deploying to production.
//const DEBUG bool = false;
const boolean DEBUG = true;

enum HellaState {
	STATE_BORED,
	STATE_ENGAGED,
	STATE_POURING,
	STATE_STOLEN,
};

enum TokenStatus {
	TOKEN_UNSET,
	TOKEN_ACCEPTED,
	TOKEN_REJECTED,
};

enum ActionDesired {
	ACTION_NONE,
	ACTION_POUR,
	ACTION_STOP,
};

class Helladuino {
public:

	HellaState state = STATE_BORED;

	String user_token = "";
	TokenStatus token_status = TOKEN_UNSET;

	boolean action_state = false;
	ActionDesired action_desired = ACTION_NONE;
	// pins.cpp will set latest_state from its ISR.
	boolean latest_state = false;

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
	void go_stolen(void);

};

#endif // __STATE_H__

