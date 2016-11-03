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

#include <SoftwareSerial.h>

// Sparkfun.com RFID Eval 13.56 MHz shield data shield indicates the pin mappings.
//   https://www.sparkfun.com/products/10406
//   https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/RFID_Eval_13.56MHz-v14.pdf
//   https://github.com/sparkfun/RFID_Evaluation_Shield/tree/V_1.4
// The UNO's Digital Pins 7 and 8 attach to D7 and D8 on the Sparkfun Shield, respectively.
SoftwareSerial rfid(7, 8);

// Prototypes.
void rfid_setup(void);
void rfid_loop(void);
//
void check_for_notag(void);
void halt(void);
void parse(void);
void print_serial(void);
void read_serial(void);
void seek(void);
void set_flag(void);

// Globals.
int flag = 0;
int Str1[11];

// Built-in Arduino one-time setup routine.
void rfid_setup()
{
	// Set the data rate for the SoftwareSerial ports
	rfid.begin(19200);
	delay(10);
	halt();
}

// Built-in Arduino main program loop.
void rfid_loop()
{
	read_serial();
}

void check_for_notag()
{
	seek();
	delay(10);
	parse();
	set_flag();

// Is this right?
//  if (flag = 1) {
	if (flag == 1) {
		seek();
		delay(10);
		parse();
	}
}

void halt()
{
	// Halt tag
	rfid.write((uint8_t)255);
	rfid.write((uint8_t)0);
	rfid.write((uint8_t)1);
	rfid.write((uint8_t)147);
	rfid.write((uint8_t)148);
}

void parse()
{
	while (rfid.available()) {
		if (rfid.read() == 255) {
			for (int i = 1; i < 11; i++) {
				Str1[i] = rfid.read();
			}
		}
	}
}

void print_serial()
{
	if (flag == 1) {
		//print to serial port
		char num[4];

		Serial.print(Str1[8], HEX);
		Serial.print(Str1[7], HEX);
		Serial.print(Str1[6], HEX);
		Serial.print(Str1[5], HEX);
		Serial.println();

		delay(100);
		//check_for_notag();
	}
}

void read_serial()
{
	seek();
	delay(10);
	parse();
	set_flag();
	print_serial();
	delay(100);
}

void seek()
{
	// Search for RFID tag.
	rfid.write((uint8_t)255);
	rfid.write((uint8_t)0);
	rfid.write((uint8_t)1);
	rfid.write((uint8_t)130);
	rfid.write((uint8_t)131);
	delay(10);
}

void set_flag()
{
	if (Str1[2] == 6) {
		flag++;
	}
	if (Str1[2] == 2) {
		flag = 0;
	}
}

