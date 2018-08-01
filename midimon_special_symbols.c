#include "midimon_special_symbols.h"

const PROGMEM uint8_t SPECIAL_SYMBOLS[SYM_COUNT][SYMBOL_WIDTH] =
{
	{ 0x02, 0x07, 0x3e, 0x00 }, // SYM_NOTE
	{ 0x30, 0x33, 0x1c, 0x30 }, // SYM_REST
	{ 0x1f, 0x11, 0x3e, 0x22 }, // SYM_CONTINUOUS_CONTROLLER
};
