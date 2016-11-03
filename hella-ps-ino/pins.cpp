#include "Arduino.h"

#include "pins.h"
#include "state.h"

volatile bool action_state = false;

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
	action_state = !action_state;
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

void pins_loop(Helladuino *hellaps) {
	// Rather than fiddle hellaps from the ISR, we just do it here,
	// on the next loop. Just make sure this loop is called before
	// the state change loop.
	hellaps->latest_state = action_state;
}

uint8_t check_steal_button(void) {
	uint8_t high_or_low = digitalRead(pins.steal_button);
	return high_or_low;
}

void pins_transition(HellaState new_state) {
// FIXME: Implement.
}

