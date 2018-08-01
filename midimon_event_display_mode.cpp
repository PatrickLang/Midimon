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
			//print_system_common(event, 1);
			break;
		}
		// Fallthrough intentional.
	case 0x04:
	case 0x06:
	case 0x07:
		//print_sysex(event.m_data, (eventId + 1) % 3 + 1);
		printSysex(renderer, event.m_data, (eventId + 1) % 3 + 1);
		break;
	case 0x02: // 2 byte sys common
	case 0x03: // 3 byte sys common
		//print_system_common(event, eventId);
		break;
	case 0x0a: // poly keypress
	case 0x0d: // channel pressure
	case 0x0e: // pitch bend
	case 0x0f: // single byte
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
