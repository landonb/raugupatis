// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include <avr/pgmspace.h>

#include "comm.h"

#include "state.h"

// Ug. I read somewhere about not using String, so now I'm
// finding myself passing char stars and buffer lengths around.
const unsigned int comm_len PROGMEM = 1024;

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
	Serial.println("Serial READY");
	this->upstream = &Serial;
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
	char payload[comm_len];
	vsnprintf_P(payload, comm_len, fmt, argp);
	// put_raw includes a newline.
	this->put_raw(payload);
	va_end(argp);
}

void CommUpstream::trace_P0(const char *fmt) {
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
	if (!DEBUG) {
		this->put_msg(PSTR("hi"));
	}
	// Command.
	if (!DEBUG) {
		this->put_msg(PSTR("trace"));
	}
	if (DEBUG) {
//		this->upstream->print("TRACE: ");
	}
	// Payload.
	this->put_raw(payload);
	// Outro.
	if (!DEBUG) {
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
		this->trace_P(PSTR("CONTRACT: Failed in file: %s / line: %d"), file, line);
	}
}

bool CommUpstream::authenticate(const char* token) {
	bool authenticated = false;

	this->trace_P(PSTR("authenticate: sending token: %s"), token);

	// Start of message.
	this->put_msg(PSTR("hi"));
	// Command.
	this->put_msg(PSTR("auth"));
	// Payload.
	this->put_raw(token);
	// Outro.
	this->put_msg(PSTR("bye"));

	// And then what? Block on read?
	// Also, some examples suggest delay(10) msecs,
	//  but we can just block on read, right?

	this->trace_P0(PSTR("authenticate: awaiting response..."));

	char response[comm_len];
	bool received = this->get_msg(response, comm_len);
	if (response[0] == '\0') {
		// FIXME: Timeout?
		this->trace_P0(PSTR("authenticate: WARNING: timeout/No response"));
	}
	else if (strcmp(response, PSTR("ok"))) {
		this->trace_P0(PSTR("authenticate: okay"));
		authenticated = true;
	}
	else if (strcmp(response, PSTR("no"))) {
		this->trace_P0(PSTR("authenticate: nope"));
	}
	else {
		this->trace_P(PSTR("authenticate: WARNING: unexpected response: %s"), response);
	}

	return authenticated;
}

void CommUpstream::update_flow(
	const char* state_name,
	unsigned long elapsed_blip,
	unsigned long elapsed_time
) {
	this->trace(PSTR("authenticate: updating flow: %s"), state_name);

	// Start of message.
	this->put_msg(PSTR("hi"));
	// Command(s) and payload.
	// - State name.
	this->put_msg(PSTR("state"));
	this->put_raw(state_name);
	// - Blips elapsed in this state.
	this->put_msg(PSTR("blips"));
	this->put_ulong(elapsed_blip);
	// - Msecs. elapsed in this state.
	this->put_msg(PSTR("msecs"));
	this->put_ulong(elapsed_time);
	// Outro.
	this->put_msg(PSTR("bye"));
}

void CommUpstream::put_raw(const char *msg) {
	if (DEBUG) {
		this->upstream->print(PSTR("MOCK/put_raw: "));
	}
	this->upstream->println(msg);

	// 2016-11-07: While investigating the serial comm garbage issue
	// (solved by using PSTR()), I tried adding flush(), but I don't
	// think it, like, waits and makes sure the outgoing bytes are
	// sent, but instead I think it just resets the channel and dumps
	// whatever is still queued to be sent. So don't do this:
	//  this->upstream->flush();
}

void CommUpstream::put_msg(const char *msg) {
	// Just keeps getting weirder: We have to do a little dance
	// so that passing PSTR() char*s to this function works,
	// otherwise we print garbage (or nothing).
	while (pgm_read_byte(msg) != '\0') {
		//this->upstream->print(pgm_read_byte(msg++));
		this->upstream->write(pgm_read_byte(msg++));
	}
	this->upstream->print('\n');
}

bool CommUpstream::get_msg(char *msg, size_t nbytes) {
	if (!DEBUG) {
		size_t bytes_read = this->upstream->readBytesUntil('\n', msg, nbytes);
		// NOTE: Don't care about bytes_read, except maybe if == nbytes.
		if (bytes_read == nbytes) {
			this->trace_P(PSTR("get_msg: WARNING: read max nbytes: %d"),  nbytes);
		}
		this->trim_msg(msg, nbytes);
	}
	else {
		// FIXME/Whatever: This is useless.
		snprintf(msg, nbytes, PSTR("MOCK/get_msg"));
	}
	return msg;
}

void CommUpstream::trim_msg(char *msg, size_t nbytes) {
	// [lb] not sure we should use strlen() in case there is not null byte.
	int msg_len = 0;
	while (msg_len < nbytes) {
		if (msg[msg_len] == '\0') {
			break;
		}
		// else, keep looking.
	}
	if (msg_len < nbytes) {
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
				this->trace_P(PSTR("get_ulong: timeout: num: %s"), num);
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

