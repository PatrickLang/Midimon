#include <usbmidi.h>
#include <midi_serialization.h>

#include "midimon.h"
#include "midimon_display.h"
#include "midimon_mode.h"

#include "UC1701.h"

void Midimon::init(IMidimonMode **modes, uint8_t n)
{
	uc1701_test();
	m_modes = modes;
	m_modeCount = n;
	m_activeModeId = 0;
	m_processFn = NULL;
	m_mode = MODE_USB_ONLY;
	m_modalMode = NULL;
	m_renderer.setDisplay(&m_display);
}

IMidimonMode * Midimon::getActiveMode() const
{
	uc1701_test();
	return !m_modalMode ? m_modes[m_activeModeId] : m_modalMode;
}

void Midimon::begin()
{
	m_display.begin();
	uc1701_test();
	return;
	m_serializerDIN5.reset();
	m_serializerUSB.reset();
	Serial.begin(31250);

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
}
