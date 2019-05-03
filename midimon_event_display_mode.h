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

#ifndef MIDIMON_EVENT_DISPLAY_MODE_H
#define MIDIMON_EVENT_DISPLAY_MODE_H

#include <stdint.h>

#include "midimon_mode.h"
#include "midimon_settings.h"

class MidimonRenderer;

class EventDisplayMode : public MidimonModeBase, public MidimonSettings::IListener
{
public:
	EventDisplayMode();

	virtual void onInit() override;
	virtual void onEnter(Midimon *midimon) override;
	virtual void onBackPressed() override;
	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) override;
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) override;

private:
	void clear();
	void newLine();
	void printEvent(MidimonRenderer &renderer, const midi_event_t &event);
	void printNote(MidimonRenderer &renderer, uint8_t ch, uint8_t note, uint8_t vel, bool on);
	void printPolyAftertouch(MidimonRenderer &renderer, uint8_t ch, uint8_t note, uint8_t pressure);
	void printAftertouch(MidimonRenderer &renderer, uint8_t ch, uint8_t pressure);
	void printPitchBend(MidimonRenderer &renderer, uint8_t ch, uint16_t value);
	void printCC(MidimonRenderer &renderer, uint8_t ch, uint8_t cc, uint8_t val);
	void printProgChange(MidimonRenderer &renderer, uint8_t ch, uint8_t prog);
	void printSysex(MidimonRenderer &renderer, const uint8_t *data, uint8_t n);
	void printSystemCommon(MidimonRenderer &renderer, const midi_event_t &event, uint8_t len);
	void printSystemRealtime(MidimonRenderer &renderer, const midi_event_t &event);

	void printRaw(MidimonRenderer &renderer, const midi_event_t &event);

	void fillRemainingPixels(MidimonRenderer &renderer);

	virtual void onChange(SettingId settingId, SettingValueType value) override;

	bool shouldFilterOut(const midi_event_t &event) const;

	uint8_t m_x;
	bool m_decodeBytes;
	bool m_filterNoisyMsg;
};

#endif // MIDIMON_EVENT_DISPLAY_MODE_H
