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

#include <Midiboy.h>
#include <usbmidi.h>

#include "midimon.h"
#include "midimon_mode.h"
#include "midimon_utils.h"
#include "midimon_event_display_mode.h"
#include "midimon_settings.h"

MODES_BEGIN(modes)
	MODES_ADD(EventDisplayMode)
MODES_END()

Midimon midimon(Midiboy, modes);

bool midimon_process(MidimonPort src, MidimonPort dst, u8 msg[3], u8 n)
{
	return true;
}

void setup()
{
	midimon.setProcessFunction(&midimon_process);
	midimon.begin();
}

void loop()
{
	midimon.poll();
}
