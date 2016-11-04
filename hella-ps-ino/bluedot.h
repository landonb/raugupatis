// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __BLUEDOT_H__
#define __BLUEDOT_H__

#include "bluedot.h"

#include "state.h"

void bluedot_setup(void);
void bluedot_reset(void);
boolean bluedot_get_key_code(uint8_t ibutton_addr[8], String& key_status);

#endif // __BLUEDOT_H__

