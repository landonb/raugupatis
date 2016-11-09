// Last Modified: 2016.11.08
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "pins.h"

// Include contract().
#include "loopers.h"

#include "comm.h"
#include "state.h"

volatile bool beerme_state_ = false;
volatile unsigned long beerme_events = 0;
volatile bool beerme_ignore = false;

volatile unsigned long flowmeter_count_ = 0;

// Setup routine.

void InputsOutputs::setup(Helladuino *hellaps) {
	this->comm = hellaps->comm;

	this->hook_beerme_button();
	this->hook_flowmeter();
	this->hook_steal_button();
	this->hook_indicator_lights();
	this->hook_annoying_alarms();
	this->hook_test_indicator();
	this->hook_beer_solenoid();
}

// ISRs.

void InputsOutputs::on_action_button_isr(void) {
	// When animating...
	if (!beerme_ignore) {
		beerme_state_ = !beerme_state_;
		beerme_events += 1;
	}
}

// FIXME/FLOWMETER: This is icing on the cake if we get everything else done.
void InputsOutputs::on_flowmeter_isr(void) {
	flowmeter_count_ += 1;
}

// Setup subroutines.

void InputsOutputs::hook_beerme_button(void) {
	// MODEs: INPUT, OUTPUT, INPUT_PULLUP.
	//   https://www.arduino.cc/en/Tutorial/DigitalPins
	pinMode(pinouts.action_button, INPUT_PULLUP);
	// Prior to Arduino 1.0.1, you'd do:
	//	pinMode(pinouts.action_button, INPUT);
	//	digitalWrite(pinouts.action_button, HIGH);

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
		digitalPinToInterrupt(pinouts.action_button),
		InputsOutputs::on_action_button_isr,
		FALLING
	);
}

void InputsOutputs::hook_flowmeter(void) {
	pinMode(pinouts.flow_meter, INPUT_PULLUP);
	attachInterrupt(
		digitalPinToInterrupt(pinouts.flow_meter),
		InputsOutputs::on_flowmeter_isr,
		FALLING
	);
}

// FIXME: INPUT, or INPUT_PULLUP?
// 2016-11-03: This button has not been tested yet.
//pinMode(pinouts.steal_button, INPUT);
void InputsOutputs::hook_steal_button(void) {
	pinMode(pinouts.steal_button, INPUT_PULLUP);
}

void InputsOutputs::hook_indicator_lights(void) {
	pinMode(pinouts.ready_indicator, OUTPUT);
	pinMode(pinouts.authed_indicator, OUTPUT);
	pinMode(pinouts.failed_indicator, OUTPUT);

	pinMode(pinouts.action_indicator, OUTPUT);
	pinMode(pinouts.steal_indicator, OUTPUT);
	//pinMode(pinouts.noise_indicator, OUTPUT);
}

void InputsOutputs::hook_annoying_alarms(void) {
	pinMode(pinouts.booze_alarm, OUTPUT);
	//pinMode(pinouts.theft_alarm, OUTPUT);
}

void InputsOutputs::hook_beer_solenoid(void) {
	pinMode(pinouts.beer_solenoid, OUTPUT);
	digitalWrite(pinouts.beer_solenoid, HIGH);
}

void InputsOutputs::hook_test_indicator(void) {
	pinMode(pinouts.test_indicator, OUTPUT);
	// Damn, the light is bright! If'n you want to test it:
	//  digitalWrite(pinouts.test_indicator, HIGH);
}

// Runtime routines.

bool InputsOutputs::get_beerme_state(void) {
	if (beerme_state_) {
		//this->comm->upstream->print("get_beerme_state: beerme_events: ");
		//this->comm->upstream->println(beerme_events);
		this->comm->trace_P("get_beerme_state: beerme_events: %lu", beerme_events);
	}
	return beerme_state_;
}

void InputsOutputs::set_beerme_state(bool new_state) {
	beerme_state_ = new_state;
}

unsigned long InputsOutputs::get_flowmeter_count(void) {
	return flowmeter_count_;
}

uint8_t InputsOutputs::check_steal_button(void) {
	uint8_t high_or_low = digitalRead(pinouts.steal_button);
	return high_or_low;
}

void InputsOutputs::transition(HellaState new_state) {
	this->animator = NULL;
	beerme_ignore = false;
	// The analogWrite duty cycle is from 0 to 255 and affects LED brightness.
	int test_indicator_duty_cycle = 15;
	// Crude. Dirty. State change.
	switch (new_state) {
		case STATE_NONE:
			// Impossible. Just bleed into STATE_BORED.
			// MAYBE: Using SRAM char* adds 42 bytes to global memory.
			//        Maybe try wrapping in PSTR.
			//        For now, just using a number instead.
			//contract(false, __FILE__, __LINE__);
			contract(false, 345, __LINE__);
		case STATE_BORED:
			digitalWrite(pinouts.ready_indicator, HIGH);
			digitalWrite(pinouts.authed_indicator, LOW);
			digitalWrite(pinouts.failed_indicator, LOW);
			digitalWrite(pinouts.action_indicator, LOW);
			digitalWrite(pinouts.steal_indicator, LOW);
			//digitalWrite(pinouts.noise_indicator, LOW);
			digitalWrite(pinouts.beer_solenoid, HIGH);
			test_indicator_duty_cycle = 15;
			break;
		case STATE_BUZZ_OFF:
			this->animator = this->animate_annoyed;
			beerme_ignore = true;
			break;
		case STATE_ENGAGING:
			break;
		case STATE_ENGAGED:
			digitalWrite(pinouts.ready_indicator, LOW);
			digitalWrite(pinouts.authed_indicator, HIGH);
			digitalWrite(pinouts.failed_indicator, LOW);
			digitalWrite(pinouts.action_indicator, LOW);
			digitalWrite(pinouts.steal_indicator, LOW);
			//digitalWrite(pinouts.noise_indicator, LOW);
			digitalWrite(pinouts.beer_solenoid, HIGH);
			test_indicator_duty_cycle = 45;
			break;
		case STATE_PATIENCE:
			break;
		case STATE_POURING:
			digitalWrite(pinouts.ready_indicator, LOW);
			digitalWrite(pinouts.authed_indicator, HIGH);
			digitalWrite(pinouts.failed_indicator, LOW);
			digitalWrite(pinouts.action_indicator, HIGH);
			digitalWrite(pinouts.steal_indicator, LOW);
			//digitalWrite(pinouts.noise_indicator, LOW);
			digitalWrite(pinouts.beer_solenoid, LOW);
			test_indicator_duty_cycle = 75;
			break;
		case STATE_GULPING:
			digitalWrite(pinouts.beer_solenoid, HIGH);
			break;
		case STATE_DEGAGING:
			break;
		case STATE_EIGHTYSIX:
			digitalWrite(pinouts.ready_indicator, LOW);
			digitalWrite(pinouts.authed_indicator, LOW);
			digitalWrite(pinouts.failed_indicator, HIGH);
			digitalWrite(pinouts.action_indicator, LOW);
			digitalWrite(pinouts.steal_indicator, LOW);
			//digitalWrite(pinouts.noise_indicator, LOW);
			digitalWrite(pinouts.beer_solenoid, HIGH);
			test_indicator_duty_cycle = 15;
			break;
		case STATE_STEALING:
			break;
		case STATE_STOLEN:
			digitalWrite(pinouts.ready_indicator, LOW);
			digitalWrite(pinouts.authed_indicator, HIGH);
			digitalWrite(pinouts.failed_indicator, LOW);
			digitalWrite(pinouts.action_indicator, LOW);
			digitalWrite(pinouts.steal_indicator, HIGH);
			//digitalWrite(pinouts.noise_indicator, HIGH);
			digitalWrite(pinouts.beer_solenoid, LOW);
			test_indicator_duty_cycle = 105;
			break;
		case STATE_SKULKING:
			break;
		default:
			// Unreachable.
			break;
	}

	analogWrite(pinouts.test_indicator, test_indicator_duty_cycle);

	this->last_animate_time = 0;
}

void InputsOutputs::animate(HellaState new_state, unsigned long state_time_0) {
	// The pins.cpp code is mostly reactive, except for a few transitional
	// states where we just let this module twiddle whatever it wants.
	if (this->animator != NULL) {
		unsigned long curr_time = millis();
		this->animator(curr_time, state_time_0, this->last_animate_time);
		this->last_animate_time = curr_time;
	}
}

void InputsOutputs::animate_annoyed(
	unsigned long curr_time,
	unsigned long state_time_0,
	unsigned long last_animate_time
) {
	if (last_animate_time == 0) {
		digitalWrite(pinouts.ready_indicator, LOW);
		digitalWrite(pinouts.authed_indicator, LOW);
		digitalWrite(pinouts.failed_indicator, HIGH);
		digitalWrite(pinouts.action_indicator, LOW);
		digitalWrite(pinouts.steal_indicator, LOW);
		//digitalWrite(pinouts.noise_indicator, LOW);
		digitalWrite(pinouts.beer_solenoid, HIGH);
	}
	else {
		unsigned long elapsed = curr_time - state_time_0;
		if (elapsed > 1000) {
			unsigned long half_sec = elapsed / 500;
			if ((half_sec % 2) == 0) {
				digitalWrite(pinouts.ready_indicator, HIGH);
				digitalWrite(pinouts.authed_indicator, HIGH);
				digitalWrite(pinouts.failed_indicator, LOW);
			}
			else {
				digitalWrite(pinouts.ready_indicator, LOW);
				digitalWrite(pinouts.authed_indicator, LOW);
				digitalWrite(pinouts.failed_indicator, HIGH);
			}
			unsigned long fifth_sec = elapsed / 200;
			if ((fifth_sec % 2) == 0) {
				digitalWrite(pinouts.action_indicator, HIGH);
				digitalWrite(pinouts.steal_indicator, LOW);
			}
			else {
				digitalWrite(pinouts.action_indicator, LOW);
				digitalWrite(pinouts.steal_indicator, HIGH);
			}
		}
	}
}

