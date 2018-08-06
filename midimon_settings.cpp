#include <avr/pgmspace.h>

#include "midimon_settings.h"
#include "midimon.h"
#include "midimon_renderer.h"
#include "midimon_input.h"

static const char STR_DECODE_HEX[]       PROGMEM = "Decode Hex";
static const char STR_MIDI_ONLY[]        PROGMEM = "DIN-5 Thru Only";
static const char STR_CONTRAST[]         PROGMEM = "Screen Contrast";
static const char STR_FILTER_NOISY_MSG[] PROGMEM = "Hide Noisy Messages";

static Setting g_settings[SETTING_COUNT] =
{
	{ STR_DECODE_HEX,       TYPE_BOOL,    1,  0, 1 },
	{ STR_MIDI_ONLY,        TYPE_BOOL,    0,  0, 1 },
	{ STR_CONTRAST,         TYPE_INTEGER, 70, 0, 100 },
	{ STR_FILTER_NOISY_MSG, TYPE_BOOL,    1,  0, 1 },
};

MidimonSettings::MidimonSettings()
{
}

void MidimonSettings::onEnter(Midimon *midimon)
{
	MidimonModeBase::onEnter(midimon);

	MidimonRenderer &renderer = midimon->getRenderer();

	for (int i=0; i<SETTING_COUNT; ++i)
	{
		renderer.setDrawPosition(0, i);
		renderer.printString_P(g_settings[i].m_title_P);
	}
}

void MidimonSettings::onExit()
{
	MidimonModeBase::onExit();
}

void MidimonSettings::onButtonEvent(MidimonButton btn, bool isDown)
{
	if (isDown && btn == BUTTON_ENTER)
		getMidimon()->exitModalMode();
}
