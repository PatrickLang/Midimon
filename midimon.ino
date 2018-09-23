#include <usbmidi.h>

#include "midimon.h"
#include "midimon_uc1701.h"
#include "midimon_mode.h"
#include "midimon_utils.h"
#include "midimon_event_display_mode.h"
#include "midimon_settings.h"

MODES_BEGIN(modes)
	MODES_ADD(EventDisplayMode)
MODES_END()

MidimonDisplay_UC1701 display;
Midimon midimon(display, modes);

bool midimon_process(MidimonPort src, MidimonPort dst, u8 msg[3])
{
	return true;
}

void setup()
{
	pinMode(LCD_BACKLIGHT, OUTPUT);
	digitalWrite(LCD_BACKLIGHT, HIGH);
	midimon.begin();
}

void loop()
{
	midimon.poll();
}
