#include "midimon_event_display_mode.h"

#include "midimon_display.h"
#include "midimon_renderer.h"

#include <midi_serialization.h>
#include <midi_messages.h>

EventDisplayMode::EventDisplayMode()
	:m_x(0)
{
}

void EventDisplayMode::onEnter(Midimon *midimon)
{
	MidimonModeBase::onEnter(midimon);

	m_x = 0;

	for (int i = 0; i<8; ++i)
		newLine();

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	renderer.setDrawPosition(21, 7);
	renderer.printChar('I');
	renderer.printChar('n');
	renderer.printChar('p');
	renderer.printChar('u');
	renderer.printChar('t');

	renderer.setDrawPosition(86, 7);
	renderer.printChar('O');
	renderer.printChar('u');
	renderer.printChar('t');
	renderer.printChar('p');
	renderer.printChar('u');
	renderer.printChar('t');
}

void EventDisplayMode::printEvent(MidimonRenderer &renderer, const midi_event_t &event)
{
	uint8_t eventId = event.m_event & 0x0f;
	switch (eventId)
	{
	case 0x08:
	case 0x09:
		printNote(renderer, event.m_data[0] & 0xf, event.m_data[1], event.m_data[2], eventId == 0x09);
		break;
	case 0x0b:
		printCC(renderer, event.m_data[0] & 0xf, event.m_data[1], event.m_data[2]);
		break;
	case 0x0c:
		printProgChange(renderer, event.m_data[0] & 0xf, event.m_data[1]);
		break;
	case 0x05:
		if (midi_is_single_byte_system_common(event.m_data[0]))
		{
			printSystemCommon(renderer, event, 1);
			break;
		}
		// Fallthrough intentional.
	case 0x04:
	case 0x06:
	case 0x07:
		printSysex(renderer, event.m_data, (eventId + 1) % 3 + 1);
		break;
	case 0x02: // 2 byte system common.
	case 0x03: // 3 byte system common.
		printSystemCommon(renderer, event, eventId);
		break;
	case 0x0f: // Single byte.
		printSystemRealtime(renderer, event);
		break;
	case 0x0a: // Aftertouch.
		printPolyAftertouch(renderer, event.m_data[0] & 0xf, event.m_data[1], event.m_data[2]);
		break;
	case 0x0d: // Channel pressure.
		printAftertouch(renderer, event.m_data[0] & 0xf, event.m_data[1]);
		break;
	case 0x0e: // Pitch bend.
		printPitchBend(renderer, event.m_data[0] & 0xf, (((uint16_t)event.m_data[2]&0x7f) << 7) | (event.m_data[1]&0x7f));
		break;
	default:
		m_x += renderer.printMidiEventHex(event);
		break;
	}

	fillRemainingPixels(renderer);
}

void EventDisplayMode::printNote(MidimonRenderer &renderer, uint8_t ch, uint8_t note, uint8_t vel, bool on)
{
	m_x += renderer.printSymbol(on ? SYM_NOTE : SYM_REST);
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printNote(note);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(vel, 3);
}

void EventDisplayMode::printPolyAftertouch(MidimonRenderer & renderer, uint8_t ch, uint8_t note, uint8_t pressure)
{
	m_x += renderer.printString("Aft.");
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printNote(note);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(pressure, 3);
}

void EventDisplayMode::printAftertouch(MidimonRenderer & renderer, uint8_t ch, uint8_t pressure)
{
	m_x += renderer.printString("Ch.Aft.");
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(pressure, 3);
}

void EventDisplayMode::printPitchBend(MidimonRenderer & renderer, uint8_t ch, uint16_t value)
{
	m_x += renderer.printString("P.Bnd. ");
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec16((int16_t)(value - 0x2000));
}

void EventDisplayMode::printCC(MidimonRenderer & renderer, uint8_t ch, uint8_t cc, uint8_t val)
{
	m_x += renderer.printSymbol(SYM_CONTINUOUS_CONTROLLER);
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(cc, 4);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(val, 3);
}

void EventDisplayMode::printProgChange(MidimonRenderer & renderer, uint8_t ch, uint8_t prog)
{
	m_x += renderer.printChar('P');
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(prog, 4);
}

void EventDisplayMode::printSysex(MidimonRenderer & renderer, const uint8_t * data, uint8_t n)
{
	m_x += renderer.printChar('S');
	m_x += renderer.printChar('X');
	m_x += renderer.printChar(' ');
	for (uint8_t i = 0; i<n; ++i)
	{
		m_x += renderer.printHex(data[i]);
		m_x += renderer.printChar(' ');
	}
}

void EventDisplayMode::printSystemCommon(MidimonRenderer &renderer, const midi_event_t &event, uint8_t len)
{
	switch (len)
	{
	case 1:
		if (event.m_data[0] == 0xf6)
		{
			m_x += renderer.printString("Tune req.");
			return;
		}
		break;
	case 2:
		if (event.m_data[0] == 0xf1)
		{
			m_x += renderer.printString("MTC");
			m_x += renderer.printDec((event.m_data[1] >> 4) & 7, 2);
			m_x += renderer.printDec((event.m_data[1] & 0xf), 3);
			return;
		}
		else if (event.m_data[0] == 0xf3)
		{
			m_x += renderer.printString("SongSel.");
			m_x += renderer.printDec(event.m_data[1] & 0x7f, 4);
			return;
		}
		break;
	case 3:
		if (event.m_data[0] == 0xf2)
		{
			m_x += renderer.printString("SPP");
			uint16_t position = (((uint16_t)event.m_data[2]&0x7f) << 7) | (event.m_data[1]&0x7f);
			uint16_t bar = position / 16 + 1;
			uint16_t measure = ((position % 16) / 4) + 1;
			m_x += renderer.printDec16(bar, 3);
			m_x += renderer.printChar(':');
			m_x += renderer.printChar(measure + '0');
			return;
		}
	default:
		break;
	}

	m_x += renderer.printMidiEventHex(event);
}

void EventDisplayMode::printSystemRealtime(MidimonRenderer & renderer, const midi_event_t &event)
{
	uint8_t byte = event.m_data[0];

	switch (byte)
	{
	case 0xf8: // Clock.
		m_x += renderer.printString("Clock");
		return;
	case 0xfa: // Start.
		m_x += renderer.printString("Start");
		return;
	case 0xfb: // Continue.
		m_x += renderer.printString("Continue");
		return;
	case 0xfc: // Stop.
		m_x += renderer.printString("Stop");
		return;
	case 0xfe: // Active sensing.
		m_x += renderer.printString("Active Sense");
		return;
	case 0xff: // Reset.
		m_x += renderer.printString("Reset");
		return;
	default:
		break;
	}

	m_x += renderer.printMidiEventHex(event);
}

void EventDisplayMode::fillRemainingPixels(MidimonRenderer &renderer)
{
	uint32_t empty = 0;
	uint8_t n = (m_x < 63 ? 63 : 128) - m_x;
	uint8_t i;
	for (i=0; i+4<n; i+=4)
		renderer.drawBitmap(&empty, 4);
	for (; i<n; ++i)
		renderer.drawBitmap(&empty, 1);
}

void EventDisplayMode::onIncomingMidiEvent(MidimonPort src, const midi_event_t &event)
{
	MidimonRenderer &renderer = getMidimon()->getRenderer();

	newLine();
	m_x = 0;
	renderer.setDrawPosition(m_x, 0);
	printEvent(renderer, event);
}

void EventDisplayMode::onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event)
{
	MidimonRenderer &renderer = getMidimon()->getRenderer();

	m_x = 65;
	renderer.setDrawPosition(m_x, 0);
	printEvent(renderer, event);
}

void EventDisplayMode::newLine()
{
	IMidimonDisplay &display = getMidimon()->getDisplay();

	uint8_t vline = 0xff;
	display.addVerticalScroll(-8);
	display.setDrawPosition(63, 0);
	display.drawBitmap(&vline, sizeof(vline));
}
