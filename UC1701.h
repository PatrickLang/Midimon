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

#ifndef UC1701_H
#define UC1701_H

#include <stdint.h>

void uc1701_init(int ss, int cd, int reset);

void uc1701_reset(); // Called by init automatically.
void uc1701_clear();

void uc1701_set_scroll(uint8_t line);
void uc1701_add_scroll(int8_t delta);
void uc1701_set_position(uint8_t x, uint8_t y); // Y is in count of 8 pixels. Scroll should be set before calling this function.
void uc1701_draw_space(uint8_t n, bool inverse); // n x 8 vertical line.
void uc1701_draw_bitmap(const void *data, uint8_t n, bool inverse); // Draw n x 8 pixels.
void uc1701_draw_progmem_bitmap(const void *data, uint8_t n, bool inverse); // Draw n x 8 pixels from PROGMEM memory.

void uc1701_set_contrast(uint8_t contrast);

#endif // UC1701_H
