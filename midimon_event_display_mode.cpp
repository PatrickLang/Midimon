#include "midimon_event_display_mode.h"

#include "midimon_display.h"
#include "midimon_renderer.h"

#include "UC1701.h"

void EventDisplayMode::onEnter(Midimon *midimon)
{
	uc1701_test();
	MidimonModeBase::onEnter(midimon);

	for (int i = 0; i<8; ++i)
		newLine();

	IMidimonDisplay &display = getMidimon()->getDisplay();
	MidimonRenderer &renderer = getMidimon()->getRenderer();

	display.setDrawPosition(21, 7);
	renderer.printChar('I');
	renderer.printChar('n');
	renderer.printChar('p');
	renderer.printChar('u');
	renderer.printChar('t');

	display.setDrawPosition(86, 7);
	renderer.printChar('O');
	renderer.printChar('u');
	renderer.printChar('t');
	renderer.printChar('p');
	renderer.printChar('u');
	renderer.printChar('t');
}

void EventDisplayMode::onIncomingMidiEvent(MidimonPort src, const midi_event_t &event)
{
}

void EventDisplayMode::onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event)
{
}

void EventDisplayMode::newLine()
{
	IMidimonDisplay &display = getMidimon()->getDisplay();
	MidimonRenderer &renderer = getMidimon()->getRenderer();

	uint8_t vline = 0xff;
	display.addVerticalScroll(-8);
	display.setDrawPosition(63, 0);
	display.drawBitmap(&vline, sizeof(vline));
}
