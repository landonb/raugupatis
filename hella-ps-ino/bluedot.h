// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __BLUEDOT_H__
#define __BLUEDOT_H__

#include "state.h"

enum Bluedot_Key_Status {
	BLUEDOT_KEY_STATUS_UNKNOWN,
	BLUEDOT_KEY_STATUS_VALID,
	BLUEDOT_KEY_STATUS_NOTHING_FOUND,
	BLUEDOT_KEY_STATUS_CRC_INVALID,
	BLUEDOT_KEY_STATUS_NOT_DS1990A,
};

class BlueDot {
public:

	void setup(void);
	void reset(void);
	Bluedot_Key_Status get_key_code(uint8_t ibutton_addr[8]);
	const char* get_key_status_name(Bluedot_Key_Status key_status);
};

#endif // __BLUEDOT_H__

