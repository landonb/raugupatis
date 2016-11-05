// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "pins.h"
#include "state.h"

volatile bool action_state = false;

// Pins setup.

void pins_setup() {
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
	// Prior to Arduino 1.0.1, you'd do:
	//	pinMode(pins.action_button, INPUT);
	//	digitalWrite(pins.action_button, HIGH);

	// The Uno hooks interrupts on Digital pins 2 and 3.
	//  https://www.arduino.cc/en/Reference/AttachInterrupt
	// Modes:
	//    LOW      triggers whenever pin is low
	//    CHANGE   triggers whenever pin changes value
	//    RISING   triggers on pin low to high
	//    FALLING  triggers on pin high to low
	//  An [se] heard on good authority from
	//  an [ee] that falling edge is most reliable.
	attachInterrupt(
		digitalPinToInterrupt(pins.action_button),
		on_action_button_isr,
		FALLING
	);
}

void on_action_button_isr(void) {
	action_state = !action_state;
}

void hook_flowmeter(void) {
	pinMode(pins.flow_meter, INPUT_PULLUP);
	attachInterrupt(
		digitalPinToInterrupt(pins.flow_meter),
		on_flowmeter_isr,
		FALLING
	);
}

void on_flowmeter_isr(void) {
	// LATER/FLOWMETER: This is icing on the cake if we get everything else done.
}

void hook_steal_button(void) {
	// FIXME: INPUT, or INPUT_PULLUP?
	// 2016-11-03: This button has not been tested yet.
	//pinMode(pins.steal_button, INPUT);
	pinMode(pins.steal_button, INPUT_PULLUP);
// FIXME: We'll need to poll this button state...
}

void hook_indicator_lights(void) {
	pinMode(pins.ready_indicator, OUTPUT);
	pinMode(pins.authed_indicator, OUTPUT);
	pinMode(pins.failed_indicator, OUTPUT);

	pinMode(pins.action_indicator, OUTPUT);
	pinMode(pins.steal_indicator, OUTPUT);
	pinMode(pins.noise_indicator, OUTPUT);
}

void hook_annoying_alarms(void) {
	pinMode(pins.booze_alarm, OUTPUT);
	pinMode(pins.theft_alarm, OUTPUT);
}

void hook_test_indicator(void) {
	pinMode(pins.test_indicator, OUTPUT);
	// Damn, the light is bright!
	//digitalWrite(pins.test_indicator, HIGH);
}

// *** Loop routines.

void pins_loop(Helladuino *hellaps) {
	// Rather than signalling hellaps from the ISR, we do it here,
	// with the understanding that hellaps calls us before going
	// into its state change routine.
	boolean was_desired = hellaps->action_desired;
	hellaps->action_desired = action_state;
	if (was_desired != hellaps->action_desired) {
		// MEH: I tried putting the ? : in the trace() but the compiler complained.
		//hellaps->trace("pins_loop: action_desired: " + hellaps->action_desired);
		hellaps->trace(hellaps->action_desired ? "true" : "false");
	}

	// The pins.cpp code is mostly reactive, except for a few transitional
	// states where we just let this module twiddle whatever it wants.
	//
// FIXME: During these state changes make lots of noise and flashy things.
	//
	int state_uptime = millis() - hellaps->state_time_0;
	switch (hellaps->state) {
		case STATE_BUZZ_OFF:
// FIXME: Make this state flash the lights for fun.
			break;
		case STATE_ENGAGING:
// FIXME: Make this state flash the lights in some way.
			break;
		case STATE_ENGAGED:
			if (state_uptime >= timeout_engaged_warning) {
// FIXME: Make this state flash the lights or whatever.
			}
			break;
		case STATE_DEGAGING:
// FIXME: Make this state flash the lights in some way.
			break;
		default:
			// Unreachable.
			break;
	}
}

uint8_t check_steal_button(void) {
	uint8_t high_or_low = digitalRead(pins.steal_button);
	return high_or_low;
}

void pins_transition(HellaState new_state) {
	// The analogWrite duty cycle is from 0 to 255 and affects LED brightness.
	int test_indicator_duty_cycle = 0;
	// Crude. Dirty. State change.
	switch (new_state) {
		case STATE_BORED:
         	digitalWrite(pins.ready_indicator, HIGH);
         	digitalWrite(pins.authed_indicator, LOW);
         	digitalWrite(pins.failed_indicator, LOW);
         	digitalWrite(pins.action_indicator, LOW);
         	digitalWrite(pins.steal_indicator, LOW);
         	digitalWrite(pins.noise_indicator, LOW);
			test_indicator_duty_cycle = 15;
			break;
		case STATE_BUZZ_OFF:
			digitalWrite(pins.ready_indicator, LOW);
			digitalWrite(pins.authed_indicator, LOW);
			digitalWrite(pins.failed_indicator, HIGH);
			digitalWrite(pins.action_indicator, LOW);
			digitalWrite(pins.steal_indicator, LOW);
			digitalWrite(pins.noise_indicator, LOW);
			test_indicator_duty_cycle = 15;
			break;
		case STATE_ENGAGED:
         	digitalWrite(pins.ready_indicator, LOW);
         	digitalWrite(pins.authed_indicator, HIGH);
         	digitalWrite(pins.failed_indicator, LOW);
         	digitalWrite(pins.action_indicator, LOW);
         	digitalWrite(pins.steal_indicator, LOW);
         	digitalWrite(pins.noise_indicator, LOW);
			test_indicator_duty_cycle = 45;
			break;
		case STATE_POURING:
         	digitalWrite(pins.ready_indicator, LOW);
         	digitalWrite(pins.authed_indicator, HIGH);
         	digitalWrite(pins.failed_indicator, LOW);
         	digitalWrite(pins.action_indicator, HIGH);
         	digitalWrite(pins.steal_indicator, LOW);
         	digitalWrite(pins.noise_indicator, LOW);
			test_indicator_duty_cycle = 75;
			break;
		case STATE_STOLEN:
         	digitalWrite(pins.ready_indicator, LOW);
         	digitalWrite(pins.authed_indicator, HIGH);
         	digitalWrite(pins.failed_indicator, LOW);
         	digitalWrite(pins.action_indicator, LOW);
         	digitalWrite(pins.steal_indicator, HIGH);
         	digitalWrite(pins.noise_indicator, HIGH);
			test_indicator_duty_cycle = 105;
			break;
		default:
			// Unreacheable.
			break;
	}

	analogWrite(pins.test_indicator, test_indicator_duty_cycle);
}

