// Last Modified: 2016.11.06
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

void CommUpstream::update_flow() {

// FIXME: Impl flowmeter and call this fcn., update_flow.

	this->trace("authenticate: updating flow: %s", token);

	// Start of message.
	this->put_msg("hi");
	// Command.
	this->put_msg("flow");
	// Payload.

// FIXME: Flowmeter: Send current count or delta or whatever.
	//this->put_byte();

	// Outro.
	this->put_msg("bye");
}

void CommUpstream::update_state(HellaState curr_state) {

// FIXME: This guy...

}

void CommUpstream::put_msg(const char *msg) {
	if (DEBUG) {
		this->upstream->print("MOCK/put_msg: ");
	}
	this->upstream->println(msg);


bool CommUpstream::get_msg(char *msg, size_t nbytes) {
	if (!DEBUG) {
		size_t bytes_read = this->upstream->readBytesUntil('\n', msg, nbytes);
		// NOTE: Don't care about bytes_read, except maybe if == nbytes.
		if (bytes_read == nbytes) {
			this->trace("get_msg: WARNING: read max nbytes: %d",  nbytes);
		}
		msg.trim();
	}
	else {
		// FIXME/Whatever: This is useless.
		snprintf(msg, nbytes, "MOCK/get_msg");
	}
	return msg;
}

void CommUpstream::put_byte(uint8_t byte) {
	if (!DEBUG) {
		this->upstream->write(byte);
// FIXME: Make sure \n rounds out; might need a fcn to help
	}
	else {
		this->upstream->print("MOCK/put_byte: 0x");
		this->upstream->println(byte, HEX);
	}
}

bool CommUpstream::get_byte(uint8_t &incoming_byte) {
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
			incoming_byte = this->upstream->read();
			got_byte = true;
		}
	}
	else {
		// FIXME/WHATEVER: This is pretty useless.
		incoming_byte = 0x00;
		this->upstream->println("MOCK/get_byte: 0xNN");
		got_byte = true;
	}
	return got_byte;
}

