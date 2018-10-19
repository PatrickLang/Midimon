/*
 * Midimon.
 * Copyright (C) 2018  Vilniaus Blokas UAB, https://blokas.io/midimon
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
