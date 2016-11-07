// Last Modified: 2016.11.07
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

	void update_flow(
		const char *state_name,
		unsigned long elapsed_blip,
		unsigned long elapsed_time
	);

	void vtrace(const char *fmt, va_list argp);
	bool contract(const bool assertion, const char *file, const unsigned long line);

private:
	bool get_byte(uint8_t &byte);
	void put_byte(uint8_t byte);
	
	bool get_msg(char *msg, size_t nbytes);
	void put_msg(const char *msg);
	void trim_msg(char *msg, size_t nbytes);

	bool get_ulong(unsigned long &num);
	void put_ulong(unsigned long num);
};

#endif // __COMM_H__

