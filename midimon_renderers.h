#ifndef MIDIMON_RENDERERS_H
#define MIDIMON_RENDERERS_H

#include "midimon.h"

class IMidimonDisplay;
struct midi_event_t;

class IMidimonRenderer
{
public:
	virtual void begin(IMidimonDisplay &display) = 0;
	virtual void handleInputEvent(MidimonPort src, const midi_event_t &event) = 0;
	virtual void handleOutputEvent(MidimonPort dst, const midi_event_t &event) = 0;
};

class EventRenderer : public IMidimonRenderer
{
public:
	EventRenderer();

	virtual void begin(IMidimonDisplay &display) override;
	virtual void handleInputEvent(MidimonPort src, const midi_event_t &event) override;
	virtual void handleOutputEvent(MidimonPort dst, const midi_event_t &event) override;

private:
	void newLine();

	IMidimonDisplay *m_display;
};

#endif // MIDIMON_RENDERERS_H
