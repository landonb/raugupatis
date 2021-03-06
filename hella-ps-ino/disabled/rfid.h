// Last Modified: 2016.11.06
// Project Page: https://github.com/landonb/raugupatis
// Adapted from code by: Aaron Weiss, aaron at sparkfun dot com
// Description: Arduino interface to RFID Eval 13.56MHz Shield.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __RFID_H__
#define __RFID_H__

// Original copy by Aaron Weiss:
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
//     D7  -> RFID RX
//     D8  -> RFID TX
//     D9  -> XBee TX
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

#include <SoftwareSerial.h>

// Prototypes.
void rfid_setup(void);
void rfid_loop(void);
//
void rfid_check_for_notag(void);
void rfid_halt(void);
void rfid_parse(void);
void rfid_print_serial(void);
bool rfid_read_serial(void);
void rfid_reset(void);
void rfid_seek(void);
void rfid_set_flag(void);

bool rfid_get_tag(int rfid_tag[4]);

#endif // __RFID_H__

