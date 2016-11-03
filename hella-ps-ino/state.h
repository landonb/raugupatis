// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __STATE_H__
#define __STATE_H__

#include "logtest.h"

class HellaPsState {

	int state;

public:
	void do_something(void);

} HellaPsState_;

void HellaPsState::do_something(void) {
	testprint(&Serial);
}

#endif // __STATE_H__

