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
int flag = 0;
int Str1[11];

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
	rfid_seek();
	delay(10);
	rfid_parse();
	rfid_set_flag();

// Is this right?
//  if (flag = 1) {
	if (flag == 1) {
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
				Str1[i] = rfid.read();
			}
		}
	}
}

void rfid_print_serial() {
	if (flag == 1) {
		//print to serial port
		char num[4];

		Serial.print(Str1[8], HEX);
		Serial.print(Str1[7], HEX);
		Serial.print(Str1[6], HEX);
		Serial.print(Str1[5], HEX);
		Serial.println();

		delay(100);
		//rfid_check_for_notag();
	}
}

void rfid_read_serial() {
	rfid_seek();
	delay(10);
	rfid_parse();
	rfid_set_flag();
	rfid_print_serial();
	delay(100);
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
	if (Str1[2] == 6) {
		flag++;
	}
	if (Str1[2] == 2) {
		flag = 0;
	}
}

void rfid_purge(void) {
}

