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

#include <Arduino.h>

#include "midimon_sh1106.h"

#include "SH1106.h"

void MidimonDisplay_SH1106::begin()
{
	sh1106_init(SS, PIN_LCD_DC, PIN_LCD_RESET);
}

void MidimonDisplay_SH1106::setDrawPosition(uint8_t x, uint8_t y_8)
{
	sh1106_set_position(x, y_8);
}

void MidimonDisplay_SH1106::setVerticalScroll(uint8_t line)
{
	sh1106_set_scroll(line);
}

void MidimonDisplay_SH1106::addVerticalScroll(int8_t lines)
{
	sh1106_add_scroll(lines);
}

void MidimonDisplay_SH1106::drawSpace(uint8_t n, bool inverse)
{
	sh1106_draw_space(n, inverse);
}

void MidimonDisplay_SH1106::drawBitmap(const void * data, uint8_t n, bool inverse)
{
	sh1106_draw_bitmap(data, n, inverse);
}

void MidimonDisplay_SH1106::drawBitmap_P(const void * data, uint8_t n, bool inverse)
{
	sh1106_draw_progmem_bitmap(data, n, inverse);
}

void MidimonDisplay_SH1106::clear()
{
	sh1106_clear();
}
