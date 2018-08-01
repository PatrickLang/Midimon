#ifndef MIDIMON_UC1701_H
#define MIDIMON_UC1701_H

#include "midimon_display.h"

class MidimonDisplay_UC1701 : public IMidimonDisplay
{
public:
	virtual void begin() override;
	virtual void setDrawPosition(uint8_t x, uint8_t y_8) override;
	virtual void setVerticalScroll(uint8_t linee) override;
	virtual void addVerticalScroll(int8_t lines) override;
	virtual void drawBitmap(const void * data, uint8_t n) override;
	virtual void drawBitmap_P(const void * data, uint8_t n) override;
};

#endif // MIDIMON_UC1701_H
