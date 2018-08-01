#ifndef MIDIMON_MODE_H
#define MIDIMON_MODE_H

#include "midimon.h"
#include "midimon_utils.h"
#include "midi_serialization.h"

class IMidimonMode
{
public:
	virtual void onEnter(Midimon *midimon) = 0;
	virtual void onExit() = 0;

	// Return true to indicate that this event overrides default Midimon behavior.
	virtual bool onButtonEvent(MidimonButton btn, bool isDown) = 0;

	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) = 0;
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) = 0;
};

class MidimonModeBase : public IMidimonMode
{
public:
	MidimonModeBase()
	{
	}

	virtual void onEnter(Midimon *midimon) override
	{
		m_midimon = midimon;
	}

	virtual void onExit() override
	{
		m_midimon = NULL;
	}

	virtual bool onButtonEvent(MidimonButton btn, bool isDown) override { return false; }

	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) override {}
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) override {}

protected:
	inline Midimon *getMidimon() const { return m_midimon; }

private:
	Midimon *m_midimon;
};

#endif // MIDIMON_MODE_H
