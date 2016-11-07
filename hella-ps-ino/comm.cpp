// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "Arduino.h"

#include "comm.h"

#include "state.h"

// Ug. I read somewhere about not using String, so now I'm
// finding myself passing char stars and buffer lengths around.
const unsigned int comm_len = 1024;

void CommUpstream::setup(void) {
	//Serial.begin(9600);
	Serial.begin(115200);
	this->upstream = &Serial;
}

void CommUpstream::trace(const char *msg, ...) {
	va_list argp;
	va_start(argp, msg);
	this->vtrace(msg, argp);
	va_end(argp);
}

void CommUpstream::vtrace(const char *fmt, va_list argp) {
	char payload[comm_len];
	// Use snprintf and not just sprintf to avoid buffer overflow.
	// NOTE: Assuming this thing gets null-terminated.
	snprintf(payload, comm_len, fmt, argp);
	// Start of message.
	this->put_msg("hi");
	// Command.
	this->put_msg("trace");
	// Payload.
	this->put_msg(payload);
	// Outro.
	this->put_msg("bye");
}

bool CommUpstream::contract(const bool assertion, const char *file, const unsigned long line) {
	if (!assertion) {
		this->trace("CONTRACT: Failed in file: %s / line: %d", file, line);
	}
}

bool CommUpstream::authenticate(const char* token) {
	bool authenticated = false;

	this->trace("authenticate: sending token: %s", token);

	// Start of message.
	this->put_msg("hi");
	// Command.
	this->put_msg("auth");
	// Payload.
	this->put_msg(token);
	// Outro.
	this->put_msg("bye");

	// And then what? Block on read?
	// Also, some examples suggest delay(10) msecs,
	//  but we can just block on read, right?
	this->trace("authenticate: awaiting response...");

	char response[comm_len];
	bool received = this->get_msg(response, comm_len);
	if (response[0] == '\0') {
		// FIXME: Timeout?
		this->trace("authenticate: WARNING: timeout/No response");
	}
	else if (strcmp(response, "ok")) {
		this->trace("authenticate: okay");
		authenticated = true;
	}
	else if (strcmp(response, "no")) {
		this->trace("authenticate: nope");
	}
	else {
		this->trace("authenticate: WARNING: unexpected response: %s", response);
	}

	return authenticated;
}

void CommUpstream::update_flow(
	const char* state_name,
	unsigned long elapsed_blip,
	unsigned long elapsed_time
) {
	this->trace("authenticate: updating flow: %s", state_name);

	// Start of message.
	this->put_msg("hi");
	// Command(s) and payload.
	// - State name.
	this->put_msg("state");
	this->put_msg(state_name);
	// - Blips elapsed in this state.
	this->put_msg("blips");
	this->put_ulong(elapsed_blip);
	// - Msecs. elapsed in this state.
	this->put_msg("msecs");
	this->put_ulong(elapsed_time);
	// Outro.
	this->put_msg("bye");
}

void CommUpstream::put_msg(const char *msg) {
	if (DEBUG) {
		this->upstream->print("MOCK/put_msg: ");
	}
	this->upstream->println(msg);
}

bool CommUpstream::get_msg(char *msg, size_t nbytes) {
	if (!DEBUG) {
		size_t bytes_read = this->upstream->readBytesUntil('\n', msg, nbytes);
		// NOTE: Don't care about bytes_read, except maybe if == nbytes.
		if (bytes_read == nbytes) {
			this->trace("get_msg: WARNING: read max nbytes: %d",  nbytes);
		}
		this->trim_msg(msg, nbytes);
	}
	else {
		// FIXME/Whatever: This is useless.
		snprintf(msg, nbytes, "MOCK/get_msg");
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
		this->upstream->print("MOCK/put_byte: ");
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
		this->upstream->println("MOCK/get_byte: 0xNN");
		got_byte = true;
	}
	return got_byte;
}

void CommUpstream::put_ulong(unsigned long num) {
	if (DEBUG) {
		this->upstream->print("MOCK/put_ulong: ");
	}
	for (int i = 0; i < 4; i++) {
		uint8_t byte = (num & 0xFF);
		num >>= 8;
		//this->upstream->print(byte, HEX);
		this->upstream->write(byte);
	}
// FIXME: Does this work to just print a newline?:
	this->upstream->println("");
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
				this->trace("get_ulong: timeout: num: %s", num);
			}
		}
	}
	else {
		// FIXME/WHATEVER: This is pretty useless.
		num = 0x00000000;
		this->upstream->println("MOCK/get_ulong: 0xNNNNNNNN");
		got_ulong = true;
	}
	return got_ulong;
}

