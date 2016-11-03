// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Adapted from code by: Aaron Weiss, aaron at sparkfun dot com
// Description: Arduino interface to RFID Eval 13.56MHz Shield.
// vim:tw=0:ts=4:sw=4:noet:

// Original copy:
//
//  RFID Eval 13.56MHz Shield example sketch v10
//
//  Aaron Weiss, aaron at sparkfun dot com
//  OSHW license: http://freedomdefined.org/OSHW
//
//   works with 13.56MHz MiFare 1k tags
//
//   Based on hardware v13:
//
//     D7 -> RFID RX
//     D8 -> RFID TX
//     D9 -> XBee TX
//     D10 -> XBee RX
//
//   Note: RFID Reset attached to D13 (aka status LED)
//
//   Note: be sure include the SoftwareSerial lib, http://arduiniana.org/libraries/newsoftserial/
//
//   Usage: Sketch prints 'Start' and waits for a tag. When a tag is in range,
//          the shield reads the tag, blinks the 'Found' LED and prints the serial
//          number of the tag to the serial port and the XBee port.
//
//   06/04/2013 - Modified for compatibility with Arudino 1.0. Seb Madgwick.

// https://www.arduino.cc/en/Main/ArduinoBoardUno

//#include <SoftwareSerial.h>

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

// Sparkfun.com RFID Eval 13.56 MHz shield data shield indicates the pin mappings.
//   https://www.sparkfun.com/products/10406
//   https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/RFID_Eval_13.56MHz-v14.pdf
//   https://github.com/sparkfun/RFID_Evaluation_Shield/tree/V_1.4
// The UNO's Digital Pins 7 and 8 attach to D7 and D8 on the Sparkfun Shield, respectively.
//SoftwareSerial rfid(7, 8);

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

