/*
 * Midimon.
 * Copyright (C) 2018  Vilniaus Blokas UAB, https://blokas.io/midimon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "midimon_input.h"
#include <fifo.h>

#include <pins_arduino.h>
#include <arduino.h>

enum { MAX_EVENTS = 8 };
enum { INPUT_DEBOUNCE_MS = 50 };
enum { INPUT_REPEAT_DELAY_MS = 300 };
enum { INPUT_REPEAT_MS = 100 };

static TFifo<InputEvent, uint8_t, MAX_EVENTS> g_eventQueue;
static uint8_t g_state;
static uint8_t g_repeating;
static unsigned long g_lastUpdated[BUTTON_COUNT];

static uint8_t readRawInput()
{
	// Read the state of the button pins directly and map them to the order of MidimonButton enum.
	return (PINC & 0x3C) >> 2;
}

static void updateState()
{
	// Read the state of the button pins directly and map them to the order of MidimonButton enum.
	uint8_t state = readRawInput();

	uint8_t diff = state ^ g_state;

	if (diff)
	{
		unsigned long ms = millis();
		for (uint8_t i=0; i<BUTTON_COUNT; ++i)
		{
			uint8_t bit = 1 << i;
			if ((diff & bit) && (ms - g_lastUpdated[i]) >= INPUT_DEBOUNCE_MS)
			{
				InputEvent e;
				e.m_button = (MidimonButton)i;
				e.m_event = (state & bit) ? EVENT_UP : EVENT_DOWN;
				g_eventQueue.push(e);
				g_lastUpdated[i] = ms;
				if (e.m_event == EVENT_UP)
				{
					g_state |= bit;
					g_repeating &= ~bit;
				}
				else
				{
					g_state &= ~bit;
				}
			}
		}
	}
}

//ISR(PCINT1_vect)
//{
//	updateState();
//}

void input_init()
{
	g_state = readRawInput();
	g_repeating = 0;
	int pins[] = { PIN_BTN_OK, PIN_BTN_BACK, PIN_BTN_UP, PIN_BTN_DOWN };

	for (int p : pins)
	{
		pinMode(p, INPUT_PULLUP);
		//*digitalPinToPCICR(p) |= bit(digitalPinToPCICRbit(p));
		//*digitalPinToPCMSK(p) |= bit(digitalPinToPCMSKbit(p));
	}
	for (int i=0; i<BUTTON_COUNT; ++i)
	{
		g_lastUpdated[i] = millis();
	}
}

void input_update()
{
	updateState();
	unsigned long ms = millis();
	for (int i=0; i<BUTTON_COUNT; ++i)
	{
		uint8_t bit = 1 << i;
		if ((g_state & bit) == 0)
		{
			if (!(g_repeating & bit))
			{
				if ((ms - g_lastUpdated[i]) >= INPUT_REPEAT_DELAY_MS)
				{
					g_repeating |= bit;
					g_lastUpdated[i] = ms;
				}
			}
			else
			{
				if ((ms - g_lastUpdated[i]) >= INPUT_REPEAT_MS)
				{
					InputEvent e;
					e.m_button = (MidimonButton)i;
					e.m_event = EVENT_DOWN;
					g_eventQueue.push(e);
					g_lastUpdated[i] = ms;
				}
			}
		}
	}
}

bool input_get_event(InputEvent &result)
{
	return g_eventQueue.pop(result);
}
