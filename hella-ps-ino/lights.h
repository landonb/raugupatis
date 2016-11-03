// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include "state.h"

// 2016-11-03: [lb] testing a simple breakout board implementation.
//
// MAGIC_NUMBERS: The UNO only supports interrupes on 2 & 3.
const int test_button = 2;
const int test_indicator = 3;

// FIXME: We'll want the Green button and the Flow meter on interrupts.
//        We can poll the Red button for pressed or not.

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

volatile bool test_state = false;
//volatile int test_state = LOW;

void lights_setup(void);
void lights_loop(HellaPsState *hellaps);
void test_button_isr(void);

void lights_setup()
{
	// MODEs: INPUT, OUTPUT, INPUT_PULLUP.
	//   https://www.arduino.cc/en/Tutorial/DigitalPins
	pinMode(test_button, INPUT_PULLUP);
	// Prior to Arduino 1.0.1, you'd have to instead do:
	//	pinMode(test_button, INPUT);
	//	digitalWrite(test_button, HIGH);

	pinMode(test_indicator, OUTPUT);
	//digitalWrite(test_indicator, HIGH);

	// Hrm, maybe I should rename this file, or should I move
	// the interrupt stuff elsewhere?

	// The Uno lets us catch interrupts on Digital pins 2 and 3.
	//  https://www.arduino.cc/en/Reference/AttachInterrupt
	// Modes:
	//  LOW to trigger the interrupt whenever the pin is low,
	//  CHANGE to trigger the interrupt whenever the pin changes value
	//  RISING to trigger when the pin goes from low to high,
	//  FALLING for when the pin goes from high to low.
	// [as] suggests triggering on the falling edge.
	attachInterrupt(digitalPinToInterrupt(test_button), test_button_isr, FALLING);
}

void lights_loop(HellaPsState *hellaps)
{
	if (false) {
		digitalWrite(test_indicator, HIGH);
		delay(200);
		digitalWrite(test_indicator, LOW);
		delay(200);
		for (int duty_cycle = 0; duty_cycle <= 255; duty_cycle++) {
			// Grow the light from dim to bright.
			analogWrite(test_indicator, duty_cycle);
			delay(50);
		}
	}

	if (!test_state) {
		hellaps->upstream->println("test_state ON");
		analogWrite(test_indicator, 50);
		delay(2000);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
		analogWrite(test_indicator, 0);
		delay(2000);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
	}
	else {
		hellaps->upstream->println("test_state OFF");
		digitalWrite(test_indicator, HIGH);
		delay(20);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
		digitalWrite(test_indicator, LOW);
		delay(20);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
	}
}

void test_button_isr(void) {
	test_state = !test_state;
}

#endif // __LIGHTS_H__

