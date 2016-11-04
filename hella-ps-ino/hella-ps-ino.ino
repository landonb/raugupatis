// Last Modified: 2016.11.04
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "loopers.h"

// This file is just a shim.
//
// - 2016-11-03: [lb]: This is my first sketch and first time using
//   the Arduino IDE, and I find myself copy-pasting this file to the
//   IDE every time I edit it, so I moved all the code to other files
//   and just edit those.
//
//   Also, after the "Upload" action, the IDE rewrites this
//   file and reformats it with 2-spaced indenting. Crazy IDE.
//
//     Unless you File > Preferences and uncheck
//      "Save when verifying or uploading"
//     but still.

// Built-in Arduino one-time setup routine.
void setup()
{
	loopers_setup();
}

// Built-in Arduino main program loop.
void loop()
{
	loopers_loop();
}

