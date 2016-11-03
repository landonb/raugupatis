// Last Modified: 2016.11.03
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#include "loopers.h"

// This file is just a shim.
//
// - [lb] is using the Arduino IDE and I find myself copy-pasting
//   this file to the IDE every time I edit it, so I moved all the
//   code to header files and will just edit those (which I find
//   that I don't have to copy-paste to the IDE).
//
//   Also, after the "Upload" action, the IDE also rewrites this
//   file and reformats it with 2-spaced indenting. Crazy IDE.
//
//   Oh, nm, I guess via File > Preferences you can also just
//   uncheck the "Save when verifying or uploading" checkbox.

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

