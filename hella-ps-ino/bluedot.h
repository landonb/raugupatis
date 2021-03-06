// Last Modified: 2016.11.08
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __BLUEDOT_H__
#define __BLUEDOT_H__

#include "state.h"

const int IBUTTON_LEN = 8;

enum Bluedot_Key_Status {
	BLUEDOT_KEY_STATUS_UNKNOWN,
	BLUEDOT_KEY_STATUS_VALID,
	BLUEDOT_KEY_STATUS_NOTHING_FOUND,
	BLUEDOT_KEY_STATUS_CRC_INVALID,
	BLUEDOT_KEY_STATUS_NOT_DS1990A,
	_BLUEDOT_KEY_STATUS_COUNT
};

class BlueDot {
public:

	void setup(void);
	void reset(void);

	Bluedot_Key_Status get_key_code(uint8_t ibutton_addr[IBUTTON_LEN]);
	const char* get_key_status_name(Bluedot_Key_Status key_status);
};

#endif // __BLUEDOT_H__

