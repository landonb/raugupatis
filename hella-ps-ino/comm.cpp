// Last Modified: 2016.11.09
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include <avr/pgmspace.h>

#include "comm.h"

#include "state.h"

// Ug. I read somewhere about not using String, so now I'm
// finding myself passing char stars and buffer lengths around.
//const unsigned int comm_len PROGMEM = 1024;
const unsigned int comm_len PROGMEM = 256;

void CommUpstream::setup(void) {
	//Serial.begin(9600);
	Serial.begin(115200);

	// 2016-11-07: While investigating a problem with weird characters
	// in the Serial output (solved by using PSTR() and not draining the
	// SRAM), I read a post that suggests waiting for the port to be hooked
	// before continuing. I cannot tell a difference with or without this wait.
	// NOTE: I read that when you hook the Arduino app with the IDE terminal,
	//       it restarts the sketch -- which is sometimes why you see part of
	//       the "Hello, Beer!" message and then see it repeated.
	while (!Serial); // While the serial stream is not open, do nothing.
	if (DEBUG) {
		Serial.println("Serial READY");
	}
	this->upstream = &Serial;


// FIXME: Is this necessary? My first "real read" keeps returning naught.
	char response[comm_len];
	bool received = this->get_msg(&response[0], comm_len);
// 2016-11-08: Crap. Didn't help...

}

void CommUpstream::trace(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	this->vtrace(fmt, argp);
	va_end(argp);
}

void CommUpstream::vtrace(const char *fmt, va_list argp) {
	char payload[comm_len];
	// Use snprintf and not just sprintf to avoid buffer overflow.
	// Correction: Use vsnprintf and not snprintf because va_list.
	vsnprintf(payload, comm_len, fmt, argp);
	this->vtrace_payload(payload);
}

// PSTR()s have to be specially treated [vsnprintf_P].
void CommUpstream::trace_P(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	this->vtrace_P(fmt, argp);
	va_end(argp);
}

void CommUpstream::write_P(const char *fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	char payload[comm_len];
	vsnprintf_P(payload, comm_len, fmt, argp);
	// put_raw includes a newline.
	this->put_raw(payload);
	va_end(argp);
}

void CommUpstream::trace_P0(const char *fmt) {
	char payload[comm_len];
	snprintf_P(payload, comm_len, fmt);
	this->vtrace_payload(payload);
}

void CommUpstream::write_P0(const char *fmt) {
	char payload[comm_len];
	snprintf_P(payload, comm_len, fmt);
	// put_raw includes a newline.
	this->put_raw(payload);
}

void CommUpstream::vtrace_P(const char *fmt, va_list argp) {
	char payload[comm_len];
	vsnprintf_P(payload, comm_len, fmt, argp);
	this->vtrace_payload(payload);
}

void CommUpstream::vtrace_payload(const char *payload) {
	// Start of message.
	if (REMOTE_TRACE) {
		this->put_msg(PSTR("hi"));
	}
	// Command.
	if (REMOTE_TRACE) {
		this->put_msg(PSTR("trace"));
	}
	// Payload.
	this->put_raw(payload);
	// Outro.
	if (REMOTE_TRACE) {
		this->put_msg(PSTR("bye"));
	}
}

/*
void CommUpstream::vtrace_P(const char *fmt, va_list argp) {
	char payload[comm_len];
	vsnprintf_P(payload, comm_len, msg, argp);
	// Start of message.
	this->put_msg(PSTR("hi"));
	// Command.
	this->put_msg(PSTR("trace"));
	// Payload.
	this->put_raw(payload);
	// Outro.
	this->put_msg(PSTR("bye"));
}
*/

bool CommUpstream::contract(const bool assertion, const char *file, const unsigned long line) {
	if (!assertion) {
		this->trace_P(PSTR("CONTRACT: Failed in file: %s / line: %lu"), file, line);
	}
}

bool CommUpstream::contract(const bool assertion, const unsigned long file, const unsigned long line) {
	if (!assertion) {
		this->trace_P(PSTR("CONTRACT: Failed in file: %lu / line: %lu"), file, line);
	}
}

bool CommUpstream::authenticate(uint8_t ibutton_addr[8]) {
	bool authenticated = false;

	//this->trace_P(PSTR("authenticate: sending token: %s"), token);

	// Start of message.
	this->put_msg(PSTR("hi"));
	// Command.
	this->put_msg(PSTR("auth"));
	// Payload.
	//this->put_raw(token);
	for (int i = 0; i < 8; i++) {
		this->upstream->write(ibutton_addr[i]);
	}
	this->upstream->println("");
	// Outro.
	this->put_msg(PSTR("bye"));

	// And then what? Block on read?
	// Also, some examples suggest delay(10) msecs,
	//  but we can just block on read, right?

	//this->trace_P0(PSTR("authenticate: awaiting response..."));

	char response[comm_len];

	// Arduino So Weird. If you call get_msg and pass the var as-is, just
	// calling this fcn. reboots the microcontroller -- and not even the
	// write_P0s above get called.
	//   bool received = this->get_msg(response, comm_len);

	// Without setting the timeout higher, the message is missed.
	// FIXME: The timeout runs to completion!
	//        The readBytesUntil does not seem to work.
	//        Maybe just expect the exact number of bytes,
	//          or read one by one.

// FIXME: This seems high...
//	Serial.setTimeout(5000);

	bool received = this->get_msg(&response[0], comm_len);

	// The normal timeout is 1000.
//	Serial.setTimeout(1000);

	if (received) {
		if (response[0] == '\0') {
			// FIXME: Timeout?
			this->trace_P0(PSTR("authenticate: WARNING: timeout/No response"));
		}
		// NOTE: If you use strcmp(), even if this code is not executed,
		//       it seems to cause this function to hang.
		else if (strcmp_P(response, PSTR("ok")) == 0) {
			this->trace_P0(PSTR("authenticate: okay"));
			authenticated = true;
		}
		else if (strcmp_P(response, PSTR("no")) == 0) {
			this->trace_P0(PSTR("authenticate: nope"));
		}
		else {
			this->trace_P(PSTR("authenticate: WARNING: unexpected response: %s"), response);
		}
	}
	else {
		this->trace_P0(PSTR("authenticate: WARNING: no response"));
	}

	return authenticated;
}

void CommUpstream::update_flow(
	const char* state_name,
	unsigned long elapsed_blip,
	unsigned long elapsed_time
) {
// FIXME/2016-11-08: Issues with this fcn.
return;

	//this->trace(PSTR("authenticate: updating flow: %s"), state_name);

	// Start of message.
	this->put_msg(PSTR("hi"));
	// Command(s) and payload.
	// - State name.
	this->put_msg(PSTR("state"));
	this->put_raw(state_name);
	// - Blips elapsed in this state.
	this->put_msg(PSTR("blips"));
	//this->put_ulong(elapsed_blip);
	this->upstream->println(elapsed_blip);
	// - Msecs. elapsed in this state.
	this->put_msg(PSTR("msecs"));
	//this->put_ulong(elapsed_time);
	this->upstream->println(elapsed_time);
	// Outro.
	this->put_msg(PSTR("bye"));
}

void CommUpstream::put_raw(const char *msg) {
	this->upstream->println(msg);

	// 2016-11-07: While investigating the serial comm garbage issue
	// (solved by using PSTR()), I tried adding flush(), but I don't
	// think it, like, waits and makes sure the outgoing bytes are
	// sent, but instead I think it just resets the channel and dumps
	// whatever is still queued to be sent. So don't do this:
	//  this->upstream->flush();
	//delay(100);
}

void CommUpstream::put_msg(const char *msg) {
	// Just keeps getting weirder: We have to do a little dance
	// so that passing PSTR() char*s to this function works,
	// otherwise we print garbage (or nothing).
	while (pgm_read_byte(msg) != '\0') {
		//this->upstream->print(pgm_read_byte(msg++));
		this->upstream->write(pgm_read_byte(msg++));
	}
	//this->upstream->print('\n');
	this->upstream->println("");
	//delay(100);
}

bool CommUpstream::get_msg(char *msg, size_t nbytes) {

	bool msg_received = false;
	msg[0] = '\0';

	if (!DEBUG) {
// FIXME: [lb] not sure, but readBytesUntil seems to ignore \n and \r triggers...
		size_t bytes_read = this->upstream->readBytesUntil('\n', msg, nbytes);
//size_t bytes_read = this->upstream->readBytesUntil('\r', msg, nbytes);
		if (bytes_read < nbytes) {
			msg[bytes_read] = '\0';
		}
		else if (nbytes > 0) {
			size_t last_byte = nbytes - 1;
			msg[last_byte] = '\0';
		}

		// NOTE: Don't care about bytes_read, except maybe if == nbytes.
		if (bytes_read > 0) {
			msg_received = true;
			if (bytes_read == nbytes) {
				this->trace_P(PSTR("get_msg: WARNING: read max nbytes: %lu"), nbytes);
			}
			this->trim_msg(msg, bytes_read);
			this->trace_P(PSTR("get_msg: got msg: %s"), msg);
		}
	}
	else {
		// FIXME/Whatever: This is useless.
		snprintf_P(msg, nbytes, PSTR("MOCK/get_msg"));
	}

	return msg_received;
}

void CommUpstream::trim_msg(char *msg, size_t nbytes) {
	// [lb] not sure we should use strlen() in case there is not null byte.
	int msg_len = 0;
	while (msg_len < nbytes) {
		if (msg[msg_len++] == '\0') {
			break;
		}
		// else, keep looking.
	}
	if ((msg_len > 0) && (msg_len < nbytes)) {
		msg_len -= 1;
		while (msg_len >= 0) {
			if (isspace(msg[msg_len])) {
				msg[msg_len] = '\0';
			}
			else {
				// All done.
				break;
			}
			msg_len -= 1;
		}
	}
}

// FIXME: put_byte and get_byte not used.
void CommUpstream::put_byte(uint8_t byte) {
	if (DEBUG) {
		this->upstream->print(PSTR("MOCK/put_byte: "));
	}
	this->upstream->println(byte, HEX);
}

// FIXME: put_byte and get_byte not used.
bool CommUpstream::get_byte(uint8_t &byte) {
	// Serial.readBytes() blocks until the timeout.
	// Serial.setTimeout(time) defaults to 1000 (milliseconds).
	// Serial.read() doesn't block as far as I [lb] can tell.
	//  https://www.arduino.cc/en/Reference/Serial
	bool got_byte = false;
	if (!DEBUG) {
		// The receive buffer holds 64 bytes until we read it.
		//  https://www.arduino.cc/en/Serial/Available
		if (this->upstream->available() == 0) {
			// Should we delay?
			//delay(500);
			delay(100);
		}
		if (this->upstream->available() > 0) {
			byte = this->upstream->read();
			got_byte = true;
		}
	}
	else {
		// FIXME/WHATEVER: This is pretty useless.
		byte = 0x00;
		this->upstream->println(PSTR("MOCK/get_byte: 0xNN"));
		got_byte = true;
	}
	return got_byte;
}

void CommUpstream::put_ulong(unsigned long num) {
	if (DEBUG) {
		this->upstream->print(PSTR("MOCK/put_ulong: "));
	}
	for (int i = 0; i < 4; i++) {
		uint8_t byte = (num & 0xFF);
		num >>= 8;
		//this->upstream->print(byte, HEX);
		this->upstream->write(byte);
	}
// FIXME: Does this work to just print a newline?:
	this->upstream->println(PSTR(""));
}

// FIXME: This fcn. is not called.
bool CommUpstream::get_ulong(unsigned long &num) {
	bool got_ulong = false;
	if (!DEBUG) {
		num = 0;
		int got_n = 0;
		while (got_n < 4) {
			if (this->upstream->available() == 0) {
				// Should we delay?
				//delay(500);
				delay(100);
			}
			if (this->upstream->available() > 0) {
				uint8_t byte = this->upstream->read();
				num |= (byte << (8 * got_n));
				got_n += 1;
				if (got_n == 4) {
					got_ulong = true;
					break;
				}
			}
			else {
				this->trace_P(PSTR("get_ulong: timeout: num: %lu"), num);
			}
		}
	}
	else {
		// FIXME/WHATEVER: This is pretty useless.
		num = 0x00000000;
		this->upstream->println(PSTR("MOCK/get_ulong: 0xNNNNNNNN"));
		got_ulong = true;
	}
	return got_ulong;
}

