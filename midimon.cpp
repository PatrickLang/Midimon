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

#include <usbmidi.h>
#include <midi_serialization.h>

#include "midimon.h"
#include "midimon_display.h"
#include "midimon_mode.h"
#include "midimon_settings.h"

#include "SH1106.h"

#include "midimon_event_display_mode.h"

USBMIDI_DEFINE_PRODUCT_NAME('M', 'i', 'd', 'i', 'm', 'o', 'n');

static MidimonSettings g_settings;

class SettingsListener : public MidimonSettings::IListener
{
public:
	SettingsListener()
		:m_midimon(NULL)
	{
	}

	void setMidimon(Midimon *midimon)
	{
		m_midimon = midimon;
	}

	virtual void onChange(SettingId settingId, SettingValueType value) override
	{
		switch (settingId)
		{
		case SETTING_OLED_BRIGHTNESS:
			sh1106_set_contrast((value << 2) + 3);
			break;
		case SETTING_MIDI_ONLY:
			m_midimon->setInterfaceMode(value != 0 ? MODE_DIN5_ONLY : MODE_USB_INTERFACE);
			break;
		default:
			break;
		}
	}

private:
	Midimon *m_midimon;
};

inline static SettingsListener &getSettingsListener()
{
	static SettingsListener settingsListener;
	return settingsListener;
}

void Midimon::init(IMidimonMode **modes, uint8_t n)
{
	m_modes = modes;
	m_modeCount = n;
	m_activeModeId = 0;
	m_processFn = NULL;
	m_mode = MODE_USB_INTERFACE;
	m_modalMode = NULL;
	m_renderer.setDisplay(&m_display);
	getSettingsListener().setMidimon(this);
}

IMidimonMode * Midimon::getActiveMode() const
{
	return !m_modalMode ? m_modes[m_activeModeId] : m_modalMode;
}

void Midimon::switchMode(uint8_t modeId)
{
	getActiveMode()->onExit();
	sh1106_clear();
	m_renderer.resetState();
	m_activeModeId = modeId;
	getActiveMode()->onEnter(this);
}

void Midimon::setProcessFunction(midimon_process_fn fn)
{
	m_processFn = fn;
}

void Midimon::begin()
{
	m_modalMode = NULL;
	m_display.begin();
	m_serializerDIN5.reset();
	m_serializerUSB.reset();
	Serial.begin(31250);

	input_init();

	g_settings.begin();

	MidimonSettings::registerListener(getSettingsListener());
	for (uint8_t i=0; i<m_modeCount; ++i)
	{
		m_modes[i]->onInit();
	}

	getActiveMode()->onEnter(this);
}

void Midimon::setInterfaceMode(MidimonInterfaceMode mode)
{
	m_serializerDIN5.reset();
	m_serializerUSB.reset();
	m_mode = mode;
}

MidimonInterfaceMode Midimon::getInterfaceMode() const
{
	return m_mode;
}

void Midimon::runModalMode(IMidimonModalMode &mode)
{
	IMidimonModalMode *previousModalMode = m_modalMode;
	getActiveMode()->onExit();
	m_modalMode = &mode;
	sh1106_clear();
	m_renderer.resetState();
	m_modalMode->onEnter(this);
	while (m_modalMode != NULL)
	{
		extern void loop();
		loop();
	}
	m_modalMode = previousModalMode;
	sh1106_clear();
	m_renderer.resetState();
	getActiveMode()->onEnter(this);
}

void Midimon::exitModalMode()
{
	m_modalMode->onExit();
	m_modalMode = NULL;
}

void Midimon::process(MidimonPort src, MidimonPort dst, Stream &input, Stream &output, MidiToUsb &serializer)
{
	MidiToUsb m2u = serializer;
	midi_event_t ev;
	int c;
	while ((c = input.read()) != -1)
	{
		if (serializer.process((u8)c, ev))
		{
			handleIncoming(src, ev);
			uint8_t len = midi_get_data_length(ev);
			if (!m_processFn)
			{
				handleOutgoing(dst, ev);
				for (uint8_t i=0; i<len; ++i)
				{
					output.write(ev.m_data[i]);
				}
			}
			else if (m_processFn(src, dst, ev.m_data, len))
			{
				midi_event_t processed;
				for (uint8_t i=0; i<3; ++i)
				{
					if (m2u.process(ev.m_data[i], processed))
					{
						handleOutgoing(dst, processed);
						for (uint8_t j=0; j<=i; ++j)
						{
							output.write(processed.m_data[j]);
						}
						break;
					}
				}
			}
			else handleOutgoing(PORT_NONE, midi_event_t());
		}
	}
}

void Midimon::handleIncoming(MidimonPort src, const midi_event_t &event)
{
	getActiveMode()->onIncomingMidiEvent(src, event);
}

void Midimon::handleOutgoing(MidimonPort dst, const midi_event_t &event)
{
	getActiveMode()->onOutgoingMidiEvent(dst, event);
}

void Midimon::poll()
{
	USBMIDI.poll();
	switch (m_mode)
	{
	case MODE_DIN5_ONLY:
		process(PORT_DIN5, PORT_DIN5, Serial, Serial, m_serializerDIN5);
		break;
	case MODE_USB_ONLY:
		process(PORT_USB, PORT_USB, USBMIDI, USBMIDI, m_serializerUSB);
		break;
	case MODE_USB_INTERFACE:
		process(PORT_DIN5, PORT_USB, Serial, USBMIDI, m_serializerDIN5);
		process(PORT_USB, PORT_DIN5, USBMIDI, Serial, m_serializerUSB);
		break;
	}
	input_update();
	InputEvent e;
	while (input_get_event(e))
	{
		if (m_modalMode == NULL)
		{
			if (e.m_event == EVENT_UP)
				continue;

			switch (e.m_button)
			{
			case BUTTON_A:
				runModalMode(g_settings);
				continue;
			case BUTTON_UP:
				switchMode((m_activeModeId + 1) % m_modeCount);
				break;
			case BUTTON_DOWN:
				switchMode((m_activeModeId + m_modeCount - 1) % m_modeCount);
				break;
			case BUTTON_B:
				getActiveMode()->onBackPressed();
				break;
			}
		}
		else
		{
			m_modalMode->onButtonEvent(e.m_button, e.m_event == EVENT_DOWN);
			if (m_modalMode && e.m_button == BUTTON_B && e.m_event == EVENT_DOWN)
				m_modalMode->onBackPressed();
		}
	}
}
