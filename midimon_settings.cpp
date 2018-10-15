#include <avr/pgmspace.h>

#include <EEPROM.h>

#include "midimon_settings.h"
#include "midimon.h"
#include "midimon_renderer.h"
#include "midimon_input.h"

MidimonSettings::IListener *MidimonSettings::s_listenerListHead = NULL;

static const char STR_ON[]               PROGMEM = "Yes";
static const char STR_OFF[]              PROGMEM = "No";

static const char STR_LCD_BACKLIGHT[]    PROGMEM = "LCD Backlight";
static const char STR_LCD_CONTRAST[]     PROGMEM = "LCD Contrast";
static const char STR_MIDI_ONLY[]        PROGMEM = "DIN-5 Thru Only";
static const char STR_DECODE_HEX[]       PROGMEM = "Decode MIDI Data";
static const char STR_FILTER_NOISY_MSG[] PROGMEM = "Hide Noisy Messages";

static Setting g_settings[SETTING_COUNT] =
{
	{ STR_LCD_BACKLIGHT,    TYPE_BOOL,    1,  0, 1 },  // SETTING_LCD_BACKLIGHT
	{ STR_LCD_CONTRAST,     TYPE_INTEGER, 40, 0, 63 }, // SETTING_LCD_CONTRAST
	{ STR_MIDI_ONLY,        TYPE_BOOL,    0,  0, 1 },  // SETTING_MIDI_ONLY
	{ STR_DECODE_HEX,       TYPE_BOOL,    1,  0, 1 },  // SETTING_DECODE_HEX
	{ STR_FILTER_NOISY_MSG, TYPE_BOOL,    1,  0, 1 },  // SETTING_FILTER_NOISY_MSG
};

enum RenderState
{
	RENDER_NORMAL,
	RENDER_HIGHLIGHTED,
	RENDER_EDITING,
};

static RenderState getRenderState(bool editing, uint8_t index, uint8_t id)
{
	return index == id ? (editing ? RENDER_EDITING : RENDER_HIGHLIGHTED) : RENDER_NORMAL;
}

static void renderSetting(MidimonRenderer &renderer, RenderState state, const Setting &setting)
{
	renderer.setInverse(state == RENDER_HIGHLIGHTED);
	renderer.drawSpace(1);
	uint8_t x = 1;
	x += renderer.printString_P(setting.m_title_P);

	switch (setting.m_type)
	{
	case TYPE_BOOL:
		x += 4 * 3;
		break;
	case TYPE_INTEGER:
		x += 4 * 6;
		break;
	default:
		x += 4;
		break;
	}

	renderer.drawSpace(128 - x - 1);
	renderer.setInverse(state != RENDER_NORMAL);
	renderer.drawSpace(1);

	switch (setting.m_type)
	{
	case TYPE_BOOL:
		if (setting.m_value)
		{
			renderer.printString_P(STR_ON);
		}
		else
		{
			renderer.printChar(' ');
			renderer.printString_P(STR_OFF);
		}
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
	:m_editing(false)
	,m_currentIndex(0)
	,m_scrollIndex(0)
{
}

void MidimonSettings::begin()
{
	loadSettingsFromEEPROM();
}

SettingValueType MidimonSettings::get(SettingId settingId)
{
	if (settingId < 0 || settingId >= SETTING_COUNT)
		return SettingValueType();

	return g_settings[settingId].m_value;
}

MidimonSettings::IListener::IListener()
	:m_next(NULL)
{
}

void MidimonSettings::registerListener(IListener &listener)
{
	// Fire current value of all settings to the new listener.
	for (int i=0; i<SETTING_COUNT; ++i)
	{
		listener.onChange((SettingId)i, g_settings[i].m_value);
	}

	if (s_listenerListHead == NULL)
	{
		s_listenerListHead = &listener;
	}
	else
	{
		IListener *l = s_listenerListHead;

		while (l->m_next != NULL)
			l = l->m_next;

		l->m_next = &listener;
	}

	listener.m_next = NULL;
}

void MidimonSettings::loadSettingsFromEEPROM()
{
	for (int i=0; i<SETTING_COUNT; ++i)
	{
		SettingValueType value;
		EEPROM.get(i * sizeof(SettingValueType), value);

		if (value != 0xffff)
		{
			if (value > g_settings[i].m_max)
				value = g_settings[i].m_max;
			else if (value < g_settings[i].m_min)
				value = g_settings[i].m_min;

			g_settings[i].m_value = value;
		}
	}
}

void MidimonSettings::saveSettingsToEEPROM()
{
	for (int i=0; i<SETTING_COUNT; ++i)
	{
		EEPROM.put(i * sizeof(SettingValueType), g_settings[i].m_value);
	}
}

void MidimonSettings::changeSetting(SettingId settingId, SettingValueType delta)
{
	if (settingId < 0 || settingId >= SETTING_COUNT)
		return;

	Setting &s = g_settings[settingId];

	SettingValueType val = s.m_value + delta;
	val = s.m_min <= val ? val : s.m_min;
	val = s.m_max >= val ? val : s.m_max;

	if (val != s.m_value)
	{
		s.m_value = val;
		fireChangeEvent(settingId, val);

		setRenderPosition(getMidimon()->getRenderer(), settingId - m_scrollIndex);
		renderSetting(getMidimon()->getRenderer(), getRenderState(m_editing, settingId, m_currentIndex), s);
	}
}

void MidimonSettings::fireChangeEvent(SettingId settingId, SettingValueType value)
{
	IListener *l = s_listenerListHead;

	while (l)
	{
		l->onChange(settingId, value);
		l = l->m_next;
	}
}

void MidimonSettings::onEnter(Midimon *midimon)
{
	MidimonModalModeBase::onEnter(midimon);

	m_editing = false;
	m_currentIndex = 0;
	m_scrollIndex = 0;

	MidimonRenderer &renderer = midimon->getRenderer();

	for (int i=SETTING_COUNT-1; i>=0; --i)
	{
		setRenderPosition(renderer, 0);
		renderSetting(renderer, getRenderState(m_editing, m_currentIndex, i), g_settings[i]);
		if (i != 0)
			renderer.addVerticalScroll(8);
	}
}

void MidimonSettings::onExit()
{
	MidimonModalModeBase::onExit();

	saveSettingsToEEPROM();
}

void MidimonSettings::onButtonEvent(MidimonButton btn, bool isDown)
{
	if (isDown)
	{
		switch (btn)
		{
		case BUTTON_ENTER:
			{
				Setting &s = g_settings[m_currentIndex];
				if (s.m_type == TYPE_BOOL)
				{
					s.m_value = !s.m_value;
					fireChangeEvent((SettingId)m_currentIndex, s.m_value);
				}
				else
				{
					m_editing = !m_editing;
				}
				setRenderPosition(getMidimon()->getRenderer(), m_currentIndex - m_scrollIndex);
				renderSetting(getMidimon()->getRenderer(), getRenderState(m_editing, m_currentIndex, m_currentIndex), g_settings[m_currentIndex]);
			}
			return;
		case BUTTON_BACK:
			if (m_editing)
			{
				m_editing = false;
				setRenderPosition(getMidimon()->getRenderer(), m_currentIndex - m_scrollIndex);
				renderSetting(getMidimon()->getRenderer(), getRenderState(m_editing, m_currentIndex, m_currentIndex), g_settings[m_currentIndex]);
			}
			else
			{
				getMidimon()->exitModalMode();
			}
			return;
		case BUTTON_UP:
			if (!m_editing)
			{
				moveCursorUp();
			}
			else
			{
				changeSetting((SettingId)m_currentIndex, +1);
			}
			return;
		case BUTTON_DOWN:
			if (!m_editing)
			{
				moveCursorDown();
			}
			else
			{
				changeSetting((SettingId)m_currentIndex, -1);
			}
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
		renderSetting(renderer, getRenderState(m_editing, m_currentIndex, m_scrollIndex), g_settings[m_scrollIndex]);
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
		renderSetting(renderer, getRenderState(m_editing, m_currentIndex, i), g_settings[i]);
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
	renderSetting(renderer, RENDER_NORMAL, g_settings[m_currentIndex]);
	--m_currentIndex;

	if (m_currentIndex < m_scrollIndex)
		scrollUp();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, RENDER_HIGHLIGHTED, g_settings[m_currentIndex]);
}

void MidimonSettings::moveCursorDown()
{
	if (m_currentIndex+1 >= SETTING_COUNT)
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, RENDER_NORMAL, g_settings[m_currentIndex]);
	++m_currentIndex;

	if (m_currentIndex - m_scrollIndex >= 8)
		scrollDown();

	setRenderPosition(renderer, m_currentIndex - m_scrollIndex);
	renderSetting(renderer, RENDER_HIGHLIGHTED, g_settings[m_currentIndex]);
}
