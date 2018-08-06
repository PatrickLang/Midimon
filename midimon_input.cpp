#include "midimon_input.h"
#include <fifo.h>

#include <pins_arduino.h>
#include <arduino.h>

enum { MAX_EVENTS = 8 };
enum { INPUT_DEBOUNCE_MS = 50 };

static TFifo<InputEvent, uint8_t, MAX_EVENTS> g_eventQueue;
static uint8_t g_state;
static unsigned long g_lastUpdated[BUTTON_COUNT];

static void updateState()
{
	// Read the state of the button pins directly and map them to the order of MidimonButton enum.
	uint8_t state = ((PIND & (1 << 6)) >> 6) | ((PINC & ((1 << 3) | (1 << 4))) >> 2);

	uint8_t diff = state ^ g_state;

	if (diff)
	{
		unsigned long ms = millis();
		for (uint8_t i=0; i<BUTTON_COUNT; ++i)
		{
			if (diff & (1 << i) && (ms - g_lastUpdated[i]) >= INPUT_DEBOUNCE_MS)
			{
				InputEvent e;
				e.m_button = (MidimonButton)i;
				e.m_event = (state & (1 << i)) ? EVENT_UP : EVENT_DOWN;
				g_eventQueue.push(e);
				g_lastUpdated[i] = ms;
			}
		}
	}

	g_state = state;
}

ISR(PCINT1_vect)
{
	updateState();
}

ISR(PCINT2_vect)
{
	updateState();
}

void input_init()
{
	g_state = 0xff;
	int pins[] = { PIN_BTN_ENTER, PIN_BTN_UP, PIN_BTN_DOWN };

	for (int p : pins)
	{
		pinMode(p, INPUT_PULLUP);
		*digitalPinToPCICR(p) |= bit(digitalPinToPCICRbit(p));
		*digitalPinToPCMSK(p) |= bit(digitalPinToPCMSKbit(p));
	}
	for (int i=0; i<BUTTON_COUNT; ++i)
	{
		g_lastUpdated[i] = micros();
	}
}

bool input_get_event(InputEvent &result)
{
	return g_eventQueue.pop(result);
}
