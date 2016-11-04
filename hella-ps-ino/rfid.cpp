// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "rfid.h"

// Sparkfun.com RFID Eval 13.56 MHz shield data shield indicates the pin mappings.
//   https://www.sparkfun.com/products/10406
//   https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/RFID_Eval_13.56MHz-v14.pdf
//   https://github.com/sparkfun/RFID_Evaluation_Shield/tree/V_1.4
//  For the read/write protocol:
//   https://www.sparkfun.com/datasheets/Sensors/ID/SM130.pdf
// The UNO's Digital Pins 7 and 8 attach to D7 and D8 on the Sparkfun Shield, respectively.
SoftwareSerial rfid(7, 8);

// Globals.
int rfid_resp[11];
boolean rfid_found = false;

// Built-in Arduino one-time setup routine.
void rfid_setup() {
	// Set the data rate for the SoftwareSerial ports
	rfid.begin(19200);
	delay(10);
	rfid_halt();
}

// Built-in Arduino main program loop.
void rfid_loop() {
	rfid_read_serial();
}

void rfid_check_for_notag() {
	// NOTE: We don't use this fcn.

	rfid_seek();
	delay(10);
	rfid_parse();

 	if (rfid_found) {
		rfid_seek();
		delay(10);
		rfid_parse();
	}
}

void rfid_halt() {
	// Halt tag.
	// From https://www.sparkfun.com/datasheets/Sensors/ID/SM130.pdf
	//   HEADER (always 0xff).
	rfid.write((uint8_t)255);
	//   Reserved (always 0x00).
	rfid.write((uint8_t)0);
	//   Payload length (in bytes) including Command and Data.
	rfid.write((uint8_t)1);
	//   Command -- 147
	rfid.write((uint8_t)147);
	//   CSUM -- Add all bytes except HEADER: 147 + 1 = 148.
	rfid.write((uint8_t)148);
}

void rfid_parse() {
	while (rfid.available()) {
		if (rfid.read() == 255) {
			for (int i = 1; i < 11; i++) {
				rfid_resp[i] = rfid.read();
			}
		}
	}

	rfid_set_flag();
}

void rfid_print_serial() {
	// NOTE: This fcn. not used.

	if (rfid_found) {
		// Print to serial port.
		Serial.print(rfid_resp[8], HEX);
		Serial.print(rfid_resp[7], HEX);
		Serial.print(rfid_resp[6], HEX);
		Serial.print(rfid_resp[5], HEX);
		Serial.println();
		//delay(100);
		//rfid_check_for_notag();
	}
}

boolean rfid_read_serial() {
	rfid_seek();
	delay(10);
	rfid_parse();
	//rfid_print_serial();
	//delay(100);
	return rfid_found;
}

boolean rfid_get_tag(int rfid_tag[4]) {
	rfid_tag[0] = 0;
	rfid_tag[1] = 0;
	rfid_tag[2] = 0;
	rfid_tag[3] = 0;

	boolean rfid_found_ = rfid_read_serial();
	if (rfid_found_) {
		rfid_tag[0] = rfid_resp[8];
		rfid_tag[1] = rfid_resp[7];
		rfid_tag[2] = rfid_resp[6];
		rfid_tag[3] = rfid_resp[5];
	}

	return rfid_found_;
}

void rfid_reset() {
	// Search for RFID tag.
	// From https://www.sparkfun.com/datasheets/Sensors/ID/SM130.pdf
	//   HEADER (always 0xff).
	rfid.write((uint8_t)255);
	//   Reserved (always 0x00).
	rfid.write((uint8_t)0);
	//   Payload length (in bytes) including Command and Data.
	rfid.write((uint8_t)1);
	//   Command -- 128/0x80 Seek for Tag
	rfid.write((uint8_t)128);
	//   CSUM -- Add all bytes except HEADER.
	rfid.write((uint8_t)129);
	// EXPLAIN: Are these delay()'s necessary for the device to respond?
	delay(10);
}

void rfid_seek() {
	// Search for RFID tag.
	// From https://www.sparkfun.com/datasheets/Sensors/ID/SM130.pdf
	//   HEADER (always 0xff).
	rfid.write((uint8_t)255);
	//   Reserved (always 0x00).
	rfid.write((uint8_t)0);
	//   Payload length (in bytes) including Command and Data.
	rfid.write((uint8_t)1);
	//   Command -- 130/0x82 Seek for Tag
	rfid.write((uint8_t)130);
	//   CSUM -- Add all bytes except HEADER.
	rfid.write((uint8_t)131);
	delay(10);
}

void rfid_set_flag() {
	// See Section 5.4 SEEK FOR TAG
	//   https://www.sparkfun.com/datasheets/Sensors/ID/SM130.pdf
	// [0] byte is typical 0xFF HEADER
	// [1] byte is typical 0x00 RESERVED
	// [2] bytes is length: 2 bytes if nothing found/6 bytes if RFID tag found.
	if (rfid_resp[2] == 6) {
		rfid_found = true;
	}
	if (rfid_resp[2] == 2) {
		rfid_found = false;
	}
}

