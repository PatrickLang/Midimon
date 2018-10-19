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

#include <Arduino.h>

#include "midimon_uc1701.h"

#include "UC1701.h"

void MidimonDisplay_UC1701::begin()
{
	uc1701_init(SS, LCD_CD, LCD_RESET);
}

void MidimonDisplay_UC1701::setDrawPosition(uint8_t x, uint8_t y_8)
{
	uc1701_set_position(x, y_8);
}

void MidimonDisplay_UC1701::setVerticalScroll(uint8_t line)
{
	uc1701_set_scroll(line);
}

void MidimonDisplay_UC1701::addVerticalScroll(int8_t lines)
{
	uc1701_add_scroll(lines);
}

void MidimonDisplay_UC1701::drawSpace(uint8_t n, bool inverse)
{
	uc1701_draw_space(n, inverse);
}

void MidimonDisplay_UC1701::drawBitmap(const void * data, uint8_t n, bool inverse)
{
	uc1701_draw_bitmap(data, n, inverse);
}

void MidimonDisplay_UC1701::drawBitmap_P(const void * data, uint8_t n, bool inverse)
{
	uc1701_draw_progmem_bitmap(data, n, inverse);
}

void MidimonDisplay_UC1701::clear()
{
	uc1701_clear();
}
