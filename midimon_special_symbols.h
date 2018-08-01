#ifndef MIDIMON_SPECIAL_SYMBOLS_H
#define MIDIMON_SPECIAL_SYMBOLS_H

#include <avr/pgmspace.h>

enum Symbol
{
	SYM_NOTE = 0,
	SYM_REST,
	SYM_CONTINUOUS_CONTROLLER,

	// Must be the last one!
	SYM_COUNT,
};

enum { SYMBOL_WIDTH=4 };

extern const PROGMEM uint8_t SPECIAL_SYMBOLS[SYM_COUNT][SYMBOL_WIDTH];

#endif // MIDIMON_SPECIAL_SYMBOLS_H
