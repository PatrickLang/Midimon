#ifndef MIDIMON_RENDERER_H
#define MIDIMON_RENDERER_H

#include <stddef.h>
#include <stdint.h>

#include "midimon_special_symbols.h"
#include "midimon_display.h"

struct midi_event_t;

class MidimonRenderer : public IMidimonDisplay
{
public:
	inline MidimonRenderer()
		:m_display(NULL)
		,m_inverse(false)
	{
	}

	inline void setDisplay(IMidimonDisplay *display) { m_display = display; }
	inline void setInverse(bool inverse) { m_inverse = inverse; }
	inline bool getInverse() const { return m_inverse; }

	void resetState();

	uint8_t printSymbol(Symbol sym);
	uint8_t printChar(char c);
	uint8_t printString(const char *str);
	uint8_t printString_P(const char *str); // PROGMEM version.
	uint8_t printHex(uint8_t val);
	uint8_t printMidiEventHex(const midi_event_t &event);
	uint8_t printDec(uint8_t val, uint8_t padding = 0);
	uint8_t printDec16(uint16_t val, uint8_t padding = 0);
	uint8_t printDec16(int16_t val, uint8_t padding = 0);
	uint8_t printNote(uint8_t note);

	virtual void begin() override;
	virtual void setDrawPosition(uint8_t x, uint8_t y_8) override;
	virtual void setVerticalScroll(uint8_t line) override;
	virtual void addVerticalScroll(int8_t lines) override;
	virtual void drawSpace(uint8_t n, bool inverse) override;
	virtual void drawBitmap(const void * data, uint8_t n, bool inverse) override;
	virtual void drawBitmap_P(const void * data, uint8_t n, bool inverse) override;

	inline void drawSpace(uint8_t n) { drawSpace(n, m_inverse); }
	inline void drawBitmap(const void * data, uint8_t n) { drawBitmap(data, n, m_inverse); }
	inline void drawBitmap_P(const void * data, uint8_t n) { drawBitmap_P(data, n, m_inverse); }

private:
	IMidimonDisplay *m_display;
	bool m_inverse;
};

#endif // MIDIMON_RENDERER_H
