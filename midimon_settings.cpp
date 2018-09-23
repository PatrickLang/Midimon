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

static void renderSetting(MidimonRenderer &renderer, bool highlighted, const Setting &setting)
{
	renderer.setInverse(highlighted);
	renderer.drawSpace(1);
	uint8_t x = 1;
	x += renderer.printString_P(setting.m_title_P);

	switch (setting.m_type)
	{
	case TYPE_BOOL:
		x += 4;
		break;
	case TYPE_INTEGER:
		x += 4 * 6;
		break;
	default:
		x += 4;
		break;
	}

	renderer.drawSpace(128 - x);

	switch (setting.m_type)
	{
	case TYPE_BOOL:
		renderer.printDec(setting.m_value != 0 ? 1 : 0);
		break;
	case TYPE_INTEGER:
		renderer.printDec16(setting.m_value, 6);
		break;
	default:
		renderer.printChar('?');
		break;
	}
}

static void setRenderPosition(MidimonRenderer &renderer, uint8_t lineIndex)
{
	if (lineIndex >= 8)
		return;

	renderer.setDrawPosition(0, 7 - lineIndex);
}

MidimonSettings::MidimonSettings()
	:m_currentIndex(0)
	,m_scrollIndex(0)
{
}

void MidimonSettings::onEnter(Midimon *midimon)
{
	MidimonModalModeBase::onEnter(midimon);

	m_currentIndex = 0;
	m_scrollIndex = 0;

	MidimonRenderer &renderer = midimon->getRenderer();

	for (int i=SETTING_COUNT-1; i>=0; --i)
	{
		setRenderPosition(renderer, 0);
		renderSetting(renderer, i == m_currentIndex, g_settings[i]);
		if (i != 0)
			renderer.addVerticalScroll(8);
	}
}

void MidimonSettings::onExit()
{
	MidimonModalModeBase::onExit();
}

void MidimonSettings::onButtonEvent(MidimonButton btn, bool isDown)
{
	if (isDown)
	{
		switch (btn)
		{
		case BUTTON_ENTER:
			//getMidimon()->exitModalMode();
			return;
		case BUTTON_BACK:
			getMidimon()->exitModalMode();
			return;
		case BUTTON_UP:
			moveCursorUp();
			return;
		case BUTTON_DOWN:
			moveCursorDown();
			return;
		}
	}
}

void MidimonSettings::scrollUp()
{
	if (SETTING_COUNT <= 8)
		return;

	if (m_scrollIndex == 0)
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	--m_scrollIndex;
	renderer.addVerticalScroll(8);
	setRenderPosition(renderer, 0);

	if (m_scrollIndex < SETTING_COUNT)
	{
		renderSetting(renderer, m_scrollIndex == m_currentIndex, g_settings[m_scrollIndex]);
	}
	else
	{
		renderer.setInverse(false);
		renderer.drawSpace(128);
	}
}

void MidimonSettings::scrollDown()
{
	if (SETTING_COUNT <= 8)
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	++m_scrollIndex;
	renderer.addVerticalScroll(-8);
	setRenderPosition(renderer, 7);

	uint8_t i = m_scrollIndex + 7;

	if (i < SETTING_COUNT)
	{
		renderSetting(renderer, i == m_currentIndex, g_settings[i]);
	}
	else
	{
		renderer.setInverse(false);
		renderer.drawSpace(128);
	}
}

void MidimonSettings::moveCursorUp()
{
	if (m_currentIndex == 0)
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, false, g_settings[m_currentIndex]);
	--m_currentIndex;

	if (m_currentIndex < m_scrollIndex)
		scrollUp();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, true, g_settings[m_currentIndex]);
}

void MidimonSettings::moveCursorDown()
{
	if (m_currentIndex+1 >= SETTING_COUNT)
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, false, g_settings[m_currentIndex]);
	++m_currentIndex;

	if (m_currentIndex - m_scrollIndex >= 8)
		scrollDown();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, true, g_settings[m_currentIndex]);
}
