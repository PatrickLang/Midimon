#include "midimon_renderer.h"
#include "midimon_font.h"
#include "midimon_display.h"
#include <midi_serialization.h>

uint8_t MidimonRenderer::printSymbol(Symbol sym)
{
	const uint8_t *p = SPECIAL_SYMBOLS[sym];
	m_display->drawBitmap_P(p, SYMBOL_WIDTH);
	return SYMBOL_WIDTH;
}

uint8_t MidimonRenderer::printChar(char c)
{
	const uint8_t *p = &MICRO_FONT[(c - ' ') * FONT_WIDTH];
	m_display->drawBitmap_P(p, FONT_WIDTH);
	uint8_t empty = 0;
	m_display->drawBitmap(&empty, sizeof(empty));
	return FONT_WIDTH + 1;
}

static const char HEX[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

uint8_t MidimonRenderer::printHex(uint8_t value)
{
	printChar(HEX[value>>4]);
	printChar(HEX[value&0xf]);
	return 2 * (FONT_WIDTH+1);
}

uint8_t MidimonRenderer::printMidiEventHex(const midi_event_t &event)
{
	uint8_t len = midi_get_data_length(event);

	if (len == 0)
	{
		printChar('-');
	}
	else
	{
		for (uint8_t i = 0; i<len; ++i)
		{
			printHex(event.m_data[i]);
			printChar(' ');
		}
	}

	return 3 * 3 * (FONT_WIDTH+1);
}

uint8_t MidimonRenderer::printDec(uint8_t val, uint8_t padding)
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

	uint8_t x = 0;

	for (int8_t i=0; i<(int8_t)(padding-n); ++i)
	{
		x += printChar(' ');
	}

	for (uint8_t i=0; i<n; ++i)
	{
		x += printChar(str[i]);
	}

	return x;
}

uint8_t MidimonRenderer::printNote(uint8_t note)
{
	if (note > 127)
		return 0;

	static const uint8_t NOTES[12] = {
		// 0x80 is used to indicate a #.
		'C', 0x80 | 'C', 'D', 0x80 | 'D', 'E', 'F', 0x80 | 'F', 'G', 0x80 | 'G', 'A', 0x80 | 'A', 'B'
	};

	char c = NOTES[note % 12];
	int8_t octave = (note / 12) - 1;

	uint8_t x = 0;

	x += printChar(c & 0x7f);
	x += printChar((c & 0x80) ? '#' : ' ');
	if (octave < 0)
	{
		x += printChar('-');
		x += printChar('1');
	}
	else
	{
		x += printDec(octave, 2);
	}
	return x;
}

void MidimonRenderer::begin()
{
	m_display->begin();
}

void MidimonRenderer::setDrawPosition(uint8_t x, uint8_t y_8)
{
	m_display->setDrawPosition(x, y_8);
}

void MidimonRenderer::setVerticalScroll(uint8_t line)
{
	m_display->setVerticalScroll(line);
}

void MidimonRenderer::addVerticalScroll(int8_t lines)
{
	m_display->addVerticalScroll(lines);
}

void MidimonRenderer::drawBitmap(const void * data, uint8_t n)
{
	m_display->drawBitmap(data, n);
}

void MidimonRenderer::drawBitmap_P(const void * data, uint8_t n)
{
	m_display->drawBitmap_P(data, n);
}
