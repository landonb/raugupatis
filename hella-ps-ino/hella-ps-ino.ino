// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "logtest.h"

#include "rfid.h"

// Devices we attach.
// - RFID scanner (2 digital pins)
// - Indicators (1 pin each):
//    Red
//    Green
//    Yellow
// - Switches
//    Red
//    Green
// - Solenoid/relay
// - Flow meter

// 2016-11-03: [lb] testing a simple breakout board implementation.
const int button_pin = 12;
const int light_pin = 3;

// Built-in Arduino one-time setup routine.
void setup()
{
  Serial.begin(9600);
  Serial.println("Start");

  testprint(&Serial);

  rfid_setup();
}

// Built-in Arduino main program loop.
void loop()
{
  rfid_loop();
}

