/*
 * Midimon.
 * Copyright (C) 2018-2019  Vilniaus Blokas UAB, https://blokas.io/midiboy
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MIDIMON_RENDERER_H
#define MIDIMON_RENDERER_H

#include <stddef.h>
#include <stdint.h>

#include <MidiboyDisplay.h>

#include "midimon_special_symbols.h"

struct midi_event_t;

class MidimonRenderer
{
public:
	inline MidimonRenderer()
		:m_display(NULL)
		,m_inverse(false)
	{
	}

	inline void setDisplay(MidiboyDisplay *display) { m_display = display; }
	inline void setInverse(bool inverse) { m_inverse = inverse; }
	inline bool getInverse() const { return m_inverse; }

	void clear();

	void resetState();

	uint8_t printSymbol(Symbol sym);
	uint8_t printChar(char c);
	uint8_t printString(const char *str);
	uint8_t printString_P(const char *str); // PROGMEM version.
	uint8_t printHex(uint8_t val);
	uint8_t printMidiEventHex(const midi_event_t &event);
	uint8_t printDec(uint8_t val, uint8_t padding = 0);
	uint8_t printDec16(uint16_t val, uint8_t padding = 0);
	uint8_t printDec16(int16_t val, uint8_t padding = 0);
	uint8_t printNote(uint8_t note);

	inline void setDrawPosition(uint8_t x, uint8_t y_8) { m_display->setDrawPosition(x, y_8); }
	inline void addVerticalScroll(int8_t delta) { m_display->addVerticalScroll(delta); }

	inline void drawSpace(uint8_t n) { m_display->drawSpace(n, m_inverse); }
	inline void drawBitmap(const void * data, uint8_t n) { m_display->drawBitmap(data, n, m_inverse); }
	inline void drawBitmap_P(const void * data, uint8_t n) { m_display->drawBitmap_P(data, n, m_inverse); }

private:
	MidiboyDisplay *m_display;
	bool m_inverse;
};

#endif // MIDIMON_RENDERER_H
