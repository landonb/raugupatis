// Last Modified: 2016.11.07
// Project Page: https://github.com/landonb/raugupatis
// Description: Ardruinko Schketch*hic*.
// vim:tw=0:ts=4:sw=4:noet:

#ifndef __HELLA_PS_H__
#define __HELLA_PS_H__

// Here we forward declare class names,
// so that those headers can include this header.
class CommUpstream;
class InputsOutputs;
class BlueDot;
class StateMachine;

// DEVs: If DEBUG true, assumes USB connected to dev machine (and not the Pi),
//       so dumps commands and traces to the line but doesn't expect responses.
const bool DEBUG = false;
//const bool DEBUG = true;

class Helladuino {
public:

	CommUpstream *comm = NULL;
	InputsOutputs *pins = NULL;
	BlueDot *bluedot = NULL;
	StateMachine *state = NULL;

	// Interface.
	void setup(void);
	void loop(void);

	void trace(const char *msg, ...);
};

#endif // __HELLA_PS_H__

