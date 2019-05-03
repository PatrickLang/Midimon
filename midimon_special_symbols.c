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

#include "midimon_special_symbols.h"

const PROGMEM uint8_t SPECIAL_SYMBOLS[SYM_COUNT][SYMBOL_WIDTH] =
{
	{ 0x02, 0x07, 0x3e, 0x00 }, // SYM_NOTE
	{ 0x30, 0x33, 0x1c, 0x30 }, // SYM_REST
	{ 0x1f, 0x11, 0x3e, 0x22 }, // SYM_CONTINUOUS_CONTROLLER
};
