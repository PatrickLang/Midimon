#include <Arduino.h>

#include "midimon_uc1701.h"

#include "UC1701.h"

void MidimonDisplay_UC1701::begin()
{
	uc1701_init(SS, LCD_CD, LCD_RESET);
}

void MidimonDisplay_UC1701::setDrawPosition(uint8_t x, uint8_t y_8)
{
	uc1701_set_position(x, y_8);
}

void MidimonDisplay_UC1701::setVerticalScroll(uint8_t line)
{
	uc1701_set_scroll(line);
}

void MidimonDisplay_UC1701::addVerticalScroll(int8_t lines)
{
	uc1701_add_scroll(lines);
}

void MidimonDisplay_UC1701::drawBitmap(const void * data, uint8_t n)
{
	uc1701_draw_bitmap(data, n);
}

void MidimonDisplay_UC1701::drawBitmap_P(const void * data, uint8_t n)
{
	uc1701_draw_progmem_bitmap(data, n);
}
