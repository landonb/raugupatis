// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include "state.h"

// 2016-11-03: [lb] testing a simple breakout board implementation.
const int button_pin = 12;
const int light_pin = 3;

// The "ready" state is when no one is logged on.
// - We're using the Yellow light for this.
const int ready_indicator = 0;
// The "authed" indicator is lit once a user has
// successfully authenticated.
// - We're using the Green light for this.
const int authed_indicator = 0;
// The "failed" indicator tells user they're not
// authenticated.
// - We're using the Red light for this.
const int failed_indicator = 0;

// The "action" button, when pressed by an authenticated
// user, activates or deactivates the service -- in our
// case, it triggers the relay attached to the solenoid
// that controls the beer flow. When pressed when a user
// is not logged on, um... we flash the failed_indicator
// or something....
// - We're using the Green button for this.
const int action_button = 0;
// The "steal" button lets a thirsty user bypass the login
// to immediately access beer. The user will likely be
// punished by loud noises and flashing lights.
// - We're using the Red button for this.
const int steal_button = 0;

// The "booze" alarm simple indicates when an authenticated
// user has pressed the action_button and activated the solenoid
// so that beer can be flowed.
// - We're using an Orange traffic construction light for this.
const int booze_alarm = 0;
const int theft_alarm = 0;

void lights_setup(void);
void lights_loop(HellaPsState *hellaps);

void lights_setup()
{
}

void lights_loop(HellaPsState *hellaps)
{
}

#endif // __LIGHTS_H__

