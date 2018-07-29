#ifndef MIDIMON_H
#define MIDIMON_H

#include <Arduino.h>

enum MidimonPort
{
	PORT_NONE, // Used for discarded output.
	PORT_DIN5,
	PORT_USB,
};

enum MidimonMode
{
	MODE_PASSTHROUGH,
	MODE_USB_INTERFACE,
};

void midimon_incoming(MidimonPort src, const u8 msg[3], bool hadStatusByte);
void midimon_outgoing(MidimonPort dst, const u8 msg[3], bool hadStatusByte);

bool midimon_process(MidimonPort src, MidimonPort dst, u8 msg[3]);

//struct ChannelNoteState
//{
//	u8 m_state[16];
//};

class Midimon_
{
public:
	Midimon_();

	void begin();

	void setMode(MidimonMode mode);
	MidimonMode getMode() const;

	//void getChannelNoteStates(ChannelNoteState &result) const;

	void poll();
};

extern Midimon_ Midimon;

#endif // MIDIMON_H
