// Last Modified: 2016.11.08
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __LOOPERS_H__
#define __LOOPERS_H__

#include "hella-ps.h"

void loopers_setup(void);
void loopers_loop(void);

void contract(const bool assertion, const char *file, const unsigned long line);

void contract(const bool assertion, const unsigned long file, const unsigned long line);

#endif // __LOOPERS_H__

