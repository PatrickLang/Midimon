#ifndef MIDIMON_SETTINGS_H
#define MIDIMON_SETTINGS_H

#include <stdint.h>

#include "midimon_mode.h"

typedef int16_t SettingValueType;

enum SettingId
{
	SETTING_LCD_BACKLIGHT    = 0, // Must be the first one, shared with bootloader!
	SETTING_LCD_CONTRAST     = 1,
	SETTING_MIDI_ONLY        = 2,
	SETTING_DECODE_HEX       = 3,
	SETTING_FILTER_NOISY_MSG = 4,

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
	class IListener
	{
	public:
		IListener();

		virtual void onChange(SettingId settingId, SettingValueType value) = 0;

	private:
		friend class MidimonSettings;
		IListener *m_next;
	};

	MidimonSettings();

	void begin();

	static SettingValueType get(SettingId settingId);
	static void registerListener(IListener &listener);

	virtual void onEnter(Midimon *midimon) override;
	virtual void onExit() override;

	virtual void onButtonEvent(MidimonButton btn, bool isDown) override;

private:
	void scrollUp();
	void scrollDown();

	void moveCursorUp();
	void moveCursorDown();

	bool m_editing;
	uint8_t m_currentIndex;
	uint8_t m_scrollIndex;

	void loadSettingsFromEEPROM();
	void saveSettingsToEEPROM();

	void changeSetting(SettingId settingId, SettingValueType delta);

	static void fireChangeEvent(SettingId settingId, SettingValueType value);

	static IListener *s_listenerListHead;
};

#endif // MIDIMON_SETTINGS_H
