#include <Arduino.h>

#include <string.h>

#include "midimon.h"
#include "midimon_display.h"
#include "midi_serialization.h"

#include "UC1701.h"
#include "font.h"

enum Symbol
{
	SYM_NOTE = 0,
	SYM_REST,
	SYM_CONTINUOUS_CONTROLLER,

	// Must be the last one!
	SYM_COUNT,
};

static const PROGMEM uint8_t SPECIAL_SYMBOLS[SYM_COUNT][4] =
{
	{ 0x02, 0x07, 0x3e, 0x00 }, // SYM_NOTE
	{ 0x30, 0x33, 0x1c, 0x30 }, // SYM_REST
	{ 0x1f, 0x11, 0x3e, 0x22 }, // SYM_CONTINUOUS_CONTROLLER
};

static uint8_t g_x;

static void print_fill_remaining()
{
	uint8_t empty = 0;
	uint8_t n = (g_x < 63 ? 63 : 128) - g_x;
	for (uint8_t i=0; i<n; ++i)
	{
		uc1701_draw_bitmap(&empty, sizeof(empty));
	}
}

static void print_char(uint8_t c)
{
	const uint8_t *p = &MICRO_FONT[(c - ' ') * 3];
	uc1701_draw_progmem_bitmap(p, 3);
	uint8_t empty = 0;
	uc1701_draw_bitmap(&empty, sizeof(empty));
	g_x += 4;
}

static void print_symbol(Symbol sym)
{
	const uint8_t *p = SPECIAL_SYMBOLS[sym];
	uc1701_draw_progmem_bitmap(p, 4);
	g_x += 4;
}

static void print_hex(uint8_t value)
{
	static const char hex[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	print_char(hex[value>>4]);
	print_char(hex[value&0xf]);
}

static void print_event_hex(midi_event_t event)
{
	uint8_t len = midi_get_data_length(event);

	if (len == 0)
	{
		print_char('-');
	}
	else
	{
		uint8_t j=0;
		for (uint8_t i=0; i<len; ++i)
		{
			print_hex(event.m_data[i]);
			print_char(' ');
		}
	}
}

static void print_dec(uint8_t val, uint8_t padding)
{
	uint8_t n = 0;
	char str[3];
	if (val < 10)
	{
		str[0] = val + '0';
		n = 1;
	}
	else if (val < 100)
	{
		str[0] = (val / 10) % 10 + '0';
		str[1] = val % 10 + '0';
		n = 2;
	}
	else
	{
		str[0] = val / 100 + '0';
		str[1] = (val / 10) % 10 + '0';
		str[2] = val % 10 + '0';
		n = 3;
	}

	for (int8_t i=0; i<(int8_t)(padding-n); ++i)
	{
		print_char(' ');
	}

	for (uint8_t i=0; i<n; ++i)
	{
		print_char(str[i]);
	}
}

static void print_note_representation(uint8_t note)
{
	static const char notes[12] = {
		// 0x80 is used to indicate a #.
		'C', 0x80 | 'C', 'D', 0x80 | 'D', 'E', 'F', 0x80 | 'F', 'G', 0x80 | 'G', 'A', 0x80 | 'A', 'B'
	};

	char c = notes[note % 12];
	int8_t octave = (note / 12) - 1;

	print_char(c & 0x7f);
	print_char((c & 0x80) ? '#' : ' ');
	if (octave < 0)
	{
		print_char('-');
		print_char('1');
	}
	else
	{
		print_dec(octave, 2);
	}
}

static void print_note(uint8_t ch, uint8_t note, uint8_t vel, bool on)
{
	print_symbol(on ? SYM_NOTE : SYM_REST);
	print_dec(ch+1, 2);
	print_char(' ');
	print_note_representation(note);
	print_char(' ');
	print_dec(vel, 3);
}

static void print_cc(uint8_t ch, uint8_t cc, uint8_t val)
{
	print_symbol(SYM_CONTINUOUS_CONTROLLER);
	print_dec(ch + 1, 2);
	print_char(' ');
	print_dec(cc, 4);
	print_char(' ');
	print_dec(val, 3);
}

static void print_prog_change(uint8_t ch, uint8_t prog)
{
	print_char('P');
	print_dec(ch + 1, 2);
	print_char(' ');
	print_dec(prog, 4);
}

static void print_sysex(uint8_t *data, uint8_t n)
{
	print_char('S');
	print_char('X');
	print_char(' ');
	for (uint8_t i=0; i<n; ++i)
	{
		print_hex(data[i]);
		print_char(' ');
	}
}

static void print_system_common(midi_event_t event, uint8_t len)
{
}

static void print_event(midi_event_t event)
{
	uint8_t eventId = event.m_event & 0x0f;
	switch (eventId)
	{
	case 0x08:
	case 0x09:
		print_note(event.m_data[0] & 0xf, event.m_data[1], event.m_data[2], eventId == 0x09);
		break;
	case 0x0b:
		print_cc(event.m_data[0] & 0xf, event.m_data[1], event.m_data[2]);
		break;
	case 0x0c:
		print_prog_change(event.m_data[0] & 0xf, event.m_data[1]);
		break;
	case 0x05:
		if (midi_is_single_byte_system_common(event.m_data[0]))
		{
			print_system_common(event, 1);
			break;
		}
		// Fallthrough intentional.
	case 0x04:
	case 0x06:
	case 0x07:
		print_sysex(event.m_data, (eventId + 1) % 3 + 1);
		break;
	case 0x02: // 2 byte sys common
	case 0x03: // 3 byte sys common
		print_system_common(event, eventId);
		break;
	case 0x0a: // poly keypress
	case 0x0d: // channel pressure
	case 0x0e: // pitch bend
	case 0x0f: // single byte
	default:
		print_event_hex(event);
		break;
	}

	print_fill_remaining();
}

void midimon_incoming(MidimonPort src, midi_event_t event, bool hadStatusByte)
{
	MidimonDisplay.newLine();
	MidimonDisplay.printInput(src, event);
}

void midimon_outgoing(MidimonPort dst, midi_event_t event, bool hadStatusByte)
{
	MidimonDisplay.printOutput(dst, event);
}

MidimonDisplay_::MidimonDisplay_()
{
}

void MidimonDisplay_::begin()
{
	uc1701_init(SS, LCD_CD, LCD_RESET);
	for (int i=0; i<8; ++i)
		newLine();

	uc1701_set_position(21, 7);
	print_char('I');
	print_char('n');
	print_char('p');
	print_char('u');
	print_char('t');

	uc1701_set_position(86, 7);
	print_char('O');
	print_char('u');
	print_char('t');
	print_char('p');
	print_char('u');
	print_char('t');
}

void MidimonDisplay_::printInput(MidimonPort port, midi_event_t event)
{
	g_x = 0;
	uc1701_set_position(0, 0);
	print_event(event);
}

void MidimonDisplay_::printOutput(MidimonPort port, midi_event_t event)
{
	g_x = 65;
	uc1701_set_position(65, 0);
	print_event(event);
}

void MidimonDisplay_::newLine()
{
	uint8_t vline = 0xff;
	uc1701_add_scroll(-8);
	uc1701_set_position(63, 0);
	uc1701_draw_bitmap(&vline, sizeof(vline));
}

void MidimonDisplay_::redraw()
{
}
