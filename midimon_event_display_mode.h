#ifndef MIDIMON_EVENT_DISPLAY_MODE_H
#define MIDIMON_EVENT_DISPLAY_MODE_H

#include "midimon_mode.h"

class EventDisplayMode : public MidimonModeBase
{
public:
	virtual void onEnter(Midimon *midimon) override;
	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) override;
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) override;

private:
	void newLine();
};

#endif // MIDIMON_EVENT_DISPLAY_MODE_H
