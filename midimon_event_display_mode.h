#ifndef MIDIMON_EVENT_DISPLAY_MODE_H
#define MIDIMON_EVENT_DISPLAY_MODE_H

#include <stdint.h>

#include "midimon_mode.h"

class MidimonRenderer;

class EventDisplayMode : public MidimonModeBase
{
public:
	EventDisplayMode();

	virtual void onEnter(Midimon *midimon) override;
	virtual void onIncomingMidiEvent(MidimonPort src, const midi_event_t &event) override;
	virtual void onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event) override;

private:
	void newLine();
	void printEvent(MidimonRenderer &renderer, const midi_event_t &event);
	void printNote(MidimonRenderer &renderer, uint8_t ch, uint8_t note, uint8_t vel, bool on);
	void printPolyAftertouch(MidimonRenderer &renderer, uint8_t ch, uint8_t note, uint8_t pressure);
	void printAftertouch(MidimonRenderer &renderer, uint8_t ch, uint8_t pressure);
	void printPitchBend(MidimonRenderer &renderer, uint8_t ch, uint16_t value);
	void printCC(MidimonRenderer &renderer, uint8_t ch, uint8_t cc, uint8_t val);
	void printProgChange(MidimonRenderer &renderer, uint8_t ch, uint8_t prog);
	void printSysex(MidimonRenderer &renderer, const uint8_t *data, uint8_t n);
	void printSystemCommon(MidimonRenderer &renderer, const midi_event_t &event, uint8_t len);
	void printSystemRealtime(MidimonRenderer &renderer, const midi_event_t &event);

	void fillRemainingPixels(MidimonRenderer &renderer);

	uint8_t m_x;
};

#endif // MIDIMON_EVENT_DISPLAY_MODE_H
