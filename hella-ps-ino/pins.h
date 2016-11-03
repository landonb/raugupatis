// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

// FIXME: Should probably rename this file. It's becoming the everything file.

#ifndef __PINS_H__
#define __PINS_H__

#include "state.h"

//struct pins {
struct {
	// MAGIC_NUMBERS: The UNO only supports interrupes on 2 & 3.
	//
	// The "action" button, when pressed by an authenticated
	// user, activates or deactivates the service -- in our
	// case, it triggers the relay attached to the solenoid
	// that controls the beer flow. When pressed when a user
	// is not logged on, um... we flash the failed_indicator
	// or something....
	// - We're using the Green button for this.
	const int action_button = 2;
	//
	// The "flow_meter" is pretty self-explanatory.
	const int flow_meter = 3;

	// The UNO supports PWM~ on ports 3, 5, 6, 9, 10, and 11,
	// which supports analogWrite(pin_num, duty_cycle) to set
	// the brigthness of an led.
	//
	// 2016-11-03: We only have only LED, and we're just using it for testing.
	const int test_indicator = 4;

	// The "steal" button lets a thirsty user bypass the login
	// to immediately access beer. The user will likely be
	// punished by loud noises and flashing lights.
	// - We're using the Red button for this.
	// NOTE: We'll need to poll this button since we're out of interrupt pins.
	const int steal_button = 13;

	// The buttons are also illuminatable.
	const int action_indicator = 6;
	const int steal_indicator = 7;
	// And we should have a noise maker for stealers.
	const int noise_indicator = 5;

	// The "ready" state is when no one is logged on.
	// - We're using the Yellow light for this.
	const int ready_indicator = 8;
	// The "authed" indicator is lit once a user has
	// successfully authenticated.
	// - We're using the Green light for this.
	const int authed_indicator = 9;
	// The "failed" indicator tells user they're not
	// authenticated.
	// - We're using the Red light for this.
	const int failed_indicator = 10;

	// The "booze" alarm simple indicates when an authenticated
	// user has pressed the action_button and activated the solenoid
	// so that beer can be flowed.
	// - We're using an Orange traffic construction light for this.
	const int booze_alarm = 11;
	const int theft_alarm = 12;

} pins;

// Setup routine.
void pins_setup(void);
// Setup subroutines.
void hook_action_button(void);
void on_action_button_isr(void);
void hook_flowmeter(void);
void on_flowmeter_isr(void);
void hook_steal_button(void);
void hook_indicator_lights(void);
void hook_annoying_alarms(void);
void hook_test_indicator(void);
// Main application loop.
void pins_loop(Helladuino *hellaps);
// Loop helpers.
uint8_t check_steal_button(void);

void pins_transition(HellaState new_state);

#endif // __PINS_H__

