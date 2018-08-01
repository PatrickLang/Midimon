#ifndef MIDIMON_RENDERER_H
#define MIDIMON_RENDERER_H

#include <stdint.h>

#include "midimon_special_symbols.h"

class IMidimonDisplay;
struct midi_event_t;

class MidimonRenderer
{
public:
	inline void setDisplay(IMidimonDisplay *display) { m_display = display; }

	uint8_t printSymbol(Symbol sym);
	uint8_t printChar(char c);
	uint8_t printHex(uint8_t val);
	uint8_t printMidiEventHex(const midi_event_t &event);
	uint8_t printDec(uint8_t val, uint8_t padding = 0);
	uint8_t printNote(uint8_t note);

private:
	IMidimonDisplay *m_display;
};

#endif // MIDIMON_RENDERER_H
