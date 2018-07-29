#include <usbmidi.h>
#include <midi_serialization.h>

#include <Arduino.h>

#include <stdint.h>

#include "midimon.h"

static MidimonMode g_mode = MODE_PASSTHROUGH;
static MidiToUsb g_serializerDIN5(0);
static MidiToUsb g_serializerUSB(0);

__attribute__((weak)) void midimon_incoming(MidimonPort src, midi_event_t event, bool hadStatusByte) {}
__attribute__((weak)) void midimon_outgoing(MidimonPort dst, midi_event_t event, bool hadStatusByte) {}

__attribute__((weak)) bool midimon_process(MidimonPort src, MidimonPort dst, u8 msg[3])
{
	return true;
}

Midimon_ Midimon;

Midimon_::Midimon_()
{
}

void Midimon_::begin()
{
	g_serializerDIN5.reset();
	g_serializerUSB.reset();
	Serial.begin(31250);
}

void Midimon_::setMode(MidimonMode mode)
{
	g_serializerDIN5.reset();
	g_serializerUSB.reset();
	g_mode = mode;
}

MidimonMode Midimon_::getMode() const
{
	return g_mode;
}

static void process(MidimonPort src, MidimonPort dst, Stream &input, Stream &output, MidiToUsb &serializer)
{
	MidiToUsb m2u = serializer;
	midi_event_t ev;
	int c;
	while ((c = input.read()) != -1)
	{
		if (serializer.process((u8)c, ev))
		{
			midimon_incoming(src, ev, true);
			if (midimon_process(src, dst, ev.m_data))
			{
				midi_event_t processed;
				for (uint8_t i=0; i<3; ++i)
				{
					if (m2u.process(ev.m_data[i], processed))
					{
						midimon_outgoing(dst, processed, true);
						for (uint8_t j=0; j<=i; ++j)
						{
							output.write(processed.m_data[j]);
						}
						break;
					}
				}
			}
			else midimon_outgoing(PORT_NONE, midi_event_t(), false);
		}
	}
}

#define Serial USBMIDI

void Midimon_::poll()
{
	USBMIDI.poll();
	switch (g_mode)
	{
	case MODE_PASSTHROUGH:
		process(PORT_DIN5, PORT_DIN5, Serial, Serial, g_serializerDIN5);
		break;
	case MODE_USB_INTERFACE:
		process(PORT_DIN5, PORT_USB, Serial, USBMIDI, g_serializerDIN5);
		process(PORT_USB, PORT_DIN5, USBMIDI, Serial, g_serializerUSB);
		break;
	}
}
