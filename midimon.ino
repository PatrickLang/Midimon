#include <usbmidi.h>

#include "midimon.h"
#include "midimon_display.h"

bool midimon_process(MidimonPort src, MidimonPort dst, u8 msg[3])
{
	return true;
}

void setup()
{
	MidimonDisplay.begin();
	Midimon.begin();

	//MidimonDisplay.print("asdf", "zcxv");
	//MidimonDisplay.print("asdf", "zcxv");
	//MidimonDisplay.print("asdf", NULL);
	//MidimonDisplay.print("asdf", NULL);
	//MidimonDisplay.print("asdf", "zcxv");
	//MidimonDisplay.print("asdf", "zcxv");
	//MidimonDisplay.print("vcxz", "fdsa");
}

void loop()
{
	Midimon.poll();
	MidimonDisplay.redraw();
}
