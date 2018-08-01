#ifndef MIDIMON_DISPLAY_H
#define MIDIMON_DISPLAY_H

#include <stdint.h>

class IMidimonDisplay
{
public:
	// Normally called from Arduino's setup() context.
	virtual void begin() = 0;

	// y_8 is the vertical page number, so provide y value in pixels divided by 8,
	// for example, to set the position to draw on vertical pixels 56-63, y_8 should be set to 7.
	// The y position is adjusted for current scroll automatically.
	virtual void setDrawPosition(uint8_t x, uint8_t y_8) = 0;

	// Recommended to scroll by 8 lines (single memory page), unless you handle updating inter-page
	// screen and offsetting data yourself.
	virtual void setVerticalScroll(uint8_t line) = 0;
	virtual void addVerticalScroll(int8_t lines) = 0;

	// Draw n x 8 pixels at current drawing position.
	virtual void drawBitmap(const void *data, uint8_t n) = 0;

	// Same as above, but for use with PROGMEM data.
	virtual void drawBitmap_P(const void *data, uint8_t n) = 0;
};

#endif // MIDIMON_DISPLAY_H
