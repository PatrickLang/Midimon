#ifndef MIDIMON_DISPLAY_H
#define MIDIMON_DISPLAY_H

#include "midimon.h"

struct midi_event_t;

class MidimonDisplay_
{
public:
	MidimonDisplay_();

	void begin();

	void printInput(MidimonPort port, midi_event_t event);
	void printOutput(MidimonPort port, midi_event_t event);
	void newLine();

	// Print in split screen mode.
	//void print(const char *input, const char *output);

	void redraw();
};

extern MidimonDisplay_ MidimonDisplay;

#endif // MIDIMON_DISPLAY_H
