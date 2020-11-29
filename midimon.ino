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

void onUsbSuspended(bool suspended)
{
	midimon.setUsbSuspended(suspended);
}

bool midimon_process(MidimonPort src, MidimonPort dst, u8 msg[3], u8 n)
{
	if (n > 0)
	{
		// Good summary of messages at https://wiki.cockos.com/wiki/index.php/MIDI_Specification
		if (msg[0] == 0xFA || // don't process MIDI start
		    msg[0] == 0xFB || //  or stop
			msg[0] == 0xFC)   //  or continue
		{
			return false;
		}
	}
	return true;
}

void setup()
{
	midimon.setProcessFunction(&midimon_process);
	midimon.begin();
	USBMIDI.setSuspendResumeCallback(&onUsbSuspended);
}

void loop()
{
	midimon.poll();
}
