#ifndef MIDIMON_INPUT_H
#define MIDIMON_INPUT_H

enum MidimonButton
{
	BUTTON_ENTER = 0,
	BUTTON_DOWN,
	BUTTON_UP,

	// Must be the last one!
	BUTTON_COUNT
};

enum EventType
{
	EVENT_DOWN,
	EVENT_UP,
};

struct InputEvent
{
	MidimonButton m_button;
	EventType m_event;
};

void input_init();
bool input_get_event(InputEvent &result);

#endif // MIDIMON_INPUT_H
