// Last Modified: 2016.11.06
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __COMM_H__
#define __COMM_H__

#include "state.h"

class CommUpstream {
public:

	// During testing, the USB wire is used for dumping trace messages.
	// When hooked up to the Raspberry Pi, it's used for talking Pibeer.
	// - When hooked up to the Pi, trace messages are sent to the Pibeer.
	HardwareSerial *upstream = NULL;

	// Interface.
	void setup(void);

	void trace(const char *msg, ...);

	// Pibeer API.

	bool authenticate(const char *token);

// FIXME: Send beer flowmeter
	void update_flow();

	void update_state(HellaState curr_state);

private:
	void vtrace(const char *fmt, va_list argp);

	bool get_byte(uint8_t &byte);
	void put_byte(uint8_t byte);
	
	bool get_msg(char *msg, size_t nbytes);
	void put_msg(const char *msg);
};

#endif // __COMM_H__

