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

#ifndef MIDIMON_DISPLAY_H
#define MIDIMON_DISPLAY_H

#include <stdint.h>

class IMidimonDisplay
{
public:
	// Normally called from Arduino's setup() context.
	virtual void begin() = 0;

	// y_8 is the vertical page number, so provide y value in pixels divided by 8,
	// for example, to set the position to draw on vertical pixels 56-63, y_8 should be set to 7.
	// The y position is adjusted for current scroll automatically.
	virtual void setDrawPosition(uint8_t x, uint8_t y_8) = 0;

	// Recommended to scroll by 8 lines (single memory page), unless you handle updating inter-page
	// screen and offsetting data yourself.
	virtual void setVerticalScroll(uint8_t line) = 0;
	virtual void addVerticalScroll(int8_t lines) = 0;

	// Draw n x 8 pixels of empty (inverse = false) or filled (inverse = true) space at current drawing position.
	virtual void drawSpace(uint8_t n , bool inverse) = 0;

	// Draw n x 8 pixels at current drawing position.
	virtual void drawBitmap(const void *data, uint8_t n, bool inverse) = 0;

	// Same as above, but for use with PROGMEM data.
	virtual void drawBitmap_P(const void *data, uint8_t n, bool inverse) = 0;

	virtual void clear() = 0;
};

#endif // MIDIMON_DISPLAY_H
