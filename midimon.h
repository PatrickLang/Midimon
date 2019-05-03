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

#ifndef MIDIMON_H
#define MIDIMON_H

#include <Arduino.h>
#include <midi_serialization.h>

#include <stdint.h>

#include "midimon_input.h"
#include "midimon_renderer.h"
#include "midimon_utils.h"

#define MODES_BEGIN(name) \
	static IMidimonMode *name[] = {

#define MODES_ADD(class) \
	static_alloc<class>(),

#define MODES_END() \
	};

class IMidimonDisplay;
class IMidimonMode;
class IMidimonModalMode;
struct midi_event_t;

enum MidimonPort
{
	PORT_NONE           = 0, // Used for discarded output.
	PORT_DIN5           = 1,
	PORT_USB            = 2,
};

enum MidimonInterfaceMode
{
	MODE_DIN5_ONLY      = 0,
	MODE_USB_ONLY       = 1,
	MODE_USB_INTERFACE  = 2,
};

typedef bool (*midimon_process_fn)(MidimonPort src, MidimonPort dst, uint8_t msg[3], uint8_t n);

class Midimon
{
public:
	template <const uint8_t count>
	Midimon(IMidimonDisplay &display, IMidimonMode *(&modes)[count])
		:m_display(display)
		,m_serializerDIN5(0)
		,m_serializerUSB(0)
	{
		init(modes, count);
	}

	Midimon(IMidimonDisplay &display, IMidimonMode **modes, uint8_t count)
		:m_display(display)
		,m_serializerDIN5(0)
		,m_serializerUSB(0)
	{
		init(modes, count);
	}

	void setProcessFunction(midimon_process_fn fn);

	void begin();
	void poll();

	void setBacklight(bool on);

	void setInterfaceMode(MidimonInterfaceMode mode);
	MidimonInterfaceMode getInterfaceMode() const;

	inline IMidimonDisplay &getDisplay() const { return m_display; }
	inline MidimonRenderer &getRenderer() { return m_renderer; }

	// Runs the loop with the given modal mode, can be used recursively for sub-modes.
	// Only Modal modes receive input events.
	void runModalMode(IMidimonModalMode &mode);
	void exitModalMode();

private:
	void init(IMidimonMode **modes, uint8_t n);
	IMidimonMode *getActiveMode() const;

	void switchMode(uint8_t modeId);

	void process(MidimonPort src, MidimonPort dst, Stream &input, Stream &output, MidiToUsb &serializer);
	void handleIncoming(MidimonPort src, const midi_event_t &event);
	void handleOutgoing(MidimonPort dst, const midi_event_t &event);

	IMidimonDisplay &m_display;
	MidimonRenderer m_renderer;
	midimon_process_fn m_processFn;

	IMidimonMode **m_modes;
	uint8_t m_modeCount;
	uint8_t m_activeModeId;
	IMidimonModalMode *m_modalMode;

	MidimonInterfaceMode m_mode;
	MidiToUsb m_serializerDIN5;
	MidiToUsb m_serializerUSB;
};

#endif // MIDIMON_H
