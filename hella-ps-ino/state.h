// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __STATE_H__
#define __STATE_H__

#include "hella-ps.h"

struct {

	const int engaging = 666;
	const int engaged_degaging = 5000;

	const int degaging = 3210;

	const int stealing = 4000;
	const int skulking = 3000;

	const int pouring_idle = 7654;

} timeouts;

enum HellaState {
	// An illegal state, just so unset/0/nil/null/NULL/None isn't valid.
	STATE_NONE,
	// The idle state is the "bored" state.
	STATE_BORED,
	// When the user swipes an RFID or iButton or authentication wand, we'll
	// transition to an intermediate state so we can do a light and sound show.
	// - If not authorized:
	STATE_BUZZ_OFF,
	// - If authorized:
	STATE_ENGAGING,
	// After the STATE_ENGAGING animation, we're engaged and the
	// user can press the Big Green Button to START_POURING beer.
	STATE_ENGAGED,
	// I guess we can't just open the solenoid on Green button press,
	// but instead we must entertain first.
	STATE_PATIENCE,
	// Galvanize! Push the button, bruh.
	// Open/close the solenoid, and send flow #s to the Pi.
	STATE_POURING,
	// For when user is done pouring and presses Green button to be done.

	
// FIXME: Let new user log on during these 2 states?
	STATE_GULPING,
	// For timing out on inactivity.
	// NOTE: User can recover from STATE_DEGAGING if they press
	//       Big Green Button and go to STATE_POURING.
	STATE_DEGAGING,


	// There are 3 ways user can logoff:
	// - 1) automatic timeout (STATE_DEGAGING)
	// - 2) another user logs on (STATE_ENGAGING)
	// - 3) authenticated user presses Big Red Button (STATE_EIGHTYSIX)
	STATE_EIGHTYSIX,
	// The special stolen state bypasses all other states.
	// - I know, right!
	STATE_STEALING,
	// STATE_STOLEN is analogous to STATE_POURING but without auth'ed user.
	STATE_STOLEN,
	// The skulking state happens when user releases the Big Red Button,
	// so that we can further humiliate them. This also delays any other
	// user or non-user (another stealer) from interacting with/engaging
	// the system while we proceed through a thoroughly obnoxious animation.
	STATE_SKULKING,
};

class StateMachine {
public:

	CommUpstream *comm = NULL;
	InputsOutputs *pins = NULL;
	BlueDot *bluedot = NULL;

	HellaState state = STATE_BORED;

	unsigned long state_time_0 = 0;

	// An authenticated user that presses the Green button enters
	// the beerme_state (and pressing button again leaves it; it's
	// a toggle button until the user is considered logged out or
	// deauthenticated or whatever we're calling it).
	bool beerme_state = false;

	// A malicious user can press and hold the Red button to bypass
	// authentication and "steal" beer, as it were.
	bool steal_state = false;

	// Use the flowmeter to track each person's pour.
	unsigned long pour_blip_0 = 0;
	unsigned long pour_time_0 = 0;
	unsigned long last_blip = 0;
	unsigned long last_time = 0;

	const char* get_state_name(void);

	void setup(Helladuino *hellaps);
	void loop(void);

	void check_stolen_state(void);
	void check_beerme_state(void);
	void check_atoken_state(void);
	void manage_current_state(void);

	void transition(HellaState new_state);

	void adjust_beerme_state(HellaState new_state);
};

#endif // __STATE_H__

