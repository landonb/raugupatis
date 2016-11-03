// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

// FIXME: Should probably rename this file. It's becoming the everything file.

#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include "state.h"

struct pins {
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
	const int test_indicator = 5;

	// The "steal" button lets a thirsty user bypass the login
	// to immediately access beer. The user will likely be
	// punished by loud noises and flashing lights.
	// - We're using the Red button for this.
	// NOTE: We'll need to poll this button since we're out of interrupt pins.
	const int steal_button = 13;

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

volatile bool test_state = false;
//volatile int test_state = LOW;

// Setup routine.
void lights_setup(void);
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
void lights_loop(Helladuino *hellaps);
// Loop helpers.
uint8_t check_steal_button(void);

void lights_setup() {
	hook_action_button();
	hook_flowmeter();
	hook_steal_button();
	hook_indicator_lights();
	hook_annoying_alarms();
	hook_test_indicator();
}

void hook_action_button(void) {
	// MODEs: INPUT, OUTPUT, INPUT_PULLUP.
	//   https://www.arduino.cc/en/Tutorial/DigitalPins
	pinMode(pins.action_button, INPUT_PULLUP);
	// Prior to Arduino 1.0.1, you'd have to instead do:
	//	pinMode(pins.action_button, INPUT);
	//	digitalWrite(pins.action_button, HIGH);

	// The Uno lets us catch interrupts on Digital pins 2 and 3.
	//  https://www.arduino.cc/en/Reference/AttachInterrupt
	// Modes:
	//  LOW to trigger the interrupt whenever the pin is low,
	//  CHANGE to trigger the interrupt whenever the pin changes value
	//  RISING to trigger when the pin goes from low to high,
	//  FALLING for when the pin goes from high to low.
	// [as] suggests triggering on the falling edge.
	attachInterrupt(digitalPinToInterrupt(pins.action_button), on_action_button_isr, FALLING);
}

void on_action_button_isr(void) {
	test_state = !test_state;
}

void hook_flowmeter(void) {
	pinMode(pins.flow_meter, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pins.flow_meter), on_flowmeter_isr, FALLING);
}

void on_flowmeter_isr(void) {
	// LATER/FLOWMETER: This is icing on the cake if we get everything else done.
}

void hook_steal_button(void) {
	// FIXME: INPUT, or INPUT_PULLUP?
	//pinMode(pins.steal_button, INPUT);
	pinMode(pins.steal_button, INPUT_PULLUP);
// FIXME: We'll need to poll this button state...
}

void hook_indicator_lights(void) {
	pinMode(pins.ready_indicator, OUTPUT);
	pinMode(pins.authed_indicator, OUTPUT);
	pinMode(pins.failed_indicator, OUTPUT);
}

void hook_annoying_alarms(void) {
	pinMode(pins.booze_alarm, OUTPUT);
	pinMode(pins.theft_alarm, OUTPUT);
}

void hook_test_indicator(void) {
	pinMode(pins.test_indicator, OUTPUT);
	//digitalWrite(pins.test_indicator, HIGH);
}

void lights_loop(Helladuino *hellaps) {
	uint8_t stealing = check_steal_button();

//	if (hellaps.state)



	hellaps->trace("YESSSSSSSSSSS");

	if (false) {
		digitalWrite(pins.test_indicator, HIGH);
		delay(200);
		digitalWrite(pins.test_indicator, LOW);
		delay(200);
		for (int duty_cycle = 0; duty_cycle <= 255; duty_cycle++) {
			// Grow the light from dim to bright.
			analogWrite(pins.test_indicator, duty_cycle);
			delay(50);
		}
	}

	if (!test_state) {
		hellaps->upstream->println("test_state ON");
		analogWrite(pins.test_indicator, 50);
		delay(2000);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
		analogWrite(pins.test_indicator, 0);
		delay(2000);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
	}
	else {
		hellaps->upstream->println("test_state OFF");
		digitalWrite(pins.test_indicator, HIGH);
		delay(20);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
		digitalWrite(pins.test_indicator, LOW);
		delay(20);
		hellaps->upstream->println(test_state ? "ON" : "OFF");
	}
}

uint8_t check_steal_button(void) {
	uint8_t high_or_low = digitalRead(pins.steal_button);
	return high_or_low;
}

#endif // __LIGHTS_H__

