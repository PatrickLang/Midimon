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

	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) = 0;
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) = 0;
};

class IMidimonModalMode : public IMidimonMode
{
public:
	virtual void onButtonEvent(MidimonButton btn, bool isDown) = 0;
};

template <class Base>
class TMidimonModeBase : public Base
{
public:
	TMidimonModeBase() {}

	virtual void onEnter(Midimon *midimon) override { m_midimon = midimon; }
	virtual void onExit() override { m_midimon = NULL; }

	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) override {}
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) override {}

protected:
	inline Midimon *getMidimon() const { return m_midimon; }

private:
	Midimon *m_midimon;
};

typedef TMidimonModeBase<IMidimonMode> MidimonModeBase;
typedef TMidimonModeBase<IMidimonModalMode> MidimonModalModeBase;

#endif // MIDIMON_MODE_H
