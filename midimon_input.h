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

#ifndef MIDIMON_INPUT_H
#define MIDIMON_INPUT_H

enum MidimonButton
{
	BUTTON_A     = 1,
	BUTTON_B     = 0,
	BUTTON_UP    = 5,
	BUTTON_DOWN  = 3,
	BUTTON_LEFT  = 4,
	BUTTON_RIGHT = 2,

	BUTTON_COUNT = 6
};

enum EventType
{
	EVENT_DOWN,
	EVENT_UP,
};

struct InputEvent
{
	MidimonButton m_button;
	EventType m_event;
};

void input_init();
void input_update();
bool input_get_event(InputEvent &result);

#endif // MIDIMON_INPUT_H
