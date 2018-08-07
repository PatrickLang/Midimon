#ifndef MIDIMON_SETTINGS_H
#define MIDIMON_SETTINGS_H

#include <stdint.h>

#include "midimon_mode.h"

typedef int16_t SettingValueType;

enum SettingId
{
	SETTING_DECODE_HEX       = 0,
	SETTING_MIDI_ONLY        = 1,
	SETTING_CONTRAST         = 2,
	SETTING_FILTER_NOISY_MSG = 3,

	// Must be the last one!
	SETTING_COUNT
};

enum SettingType
{
	TYPE_BOOL,
	TYPE_INTEGER,
};

struct Setting
{
	const char *m_title_P; // Pointer to progmem.
	SettingType m_type;
	SettingValueType m_value;
	SettingValueType m_min;
	SettingValueType m_max;
};

class MidimonSettings : public MidimonModalModeBase
{
public:
	MidimonSettings();

	virtual void onEnter(Midimon *midimon) override;
	virtual void onExit() override;

	virtual void onButtonEvent(MidimonButton btn, bool isDown) override;

private:
	void scrollUp();
	void scrollDown();

	void moveCursorUp();
	void moveCursorDown();

	uint8_t m_currentIndex;
	uint8_t m_scrollIndex;
};

#endif // MIDIMON_SETTINGS_H
