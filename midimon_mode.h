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

#ifndef MIDIMON_MODE_H
#define MIDIMON_MODE_H

#include "midimon.h"
#include "midimon_utils.h"
#include "midi_serialization.h"

class IMidimonMode
{
public:
	virtual void onInit() = 0;

	virtual void onEnter(Midimon *midimon) = 0;
	virtual void onExit() = 0;

	// Only input reported to regular modes.
	virtual void onBackPressed() = 0;

	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) = 0;
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) = 0;
};

class IMidimonModalMode : public IMidimonMode
{
public:
	virtual void onButtonEvent(MidimonButton btn, bool isDown) = 0;
};

template <class Base>
class TMidimonModeBase : public Base
{
public:
	TMidimonModeBase() {}

	virtual void onInit() override {}

	virtual void onEnter(Midimon *midimon) override { m_midimon = midimon; }
	virtual void onExit() override { m_midimon = NULL; }

	virtual void onBackPressed() override {}

	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) override {}
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) override {}

protected:
	inline Midimon *getMidimon() const { return m_midimon; }

private:
	Midimon *m_midimon;
};

typedef TMidimonModeBase<IMidimonMode> MidimonModeBase;
typedef TMidimonModeBase<IMidimonModalMode> MidimonModalModeBase;

#endif // MIDIMON_MODE_H
