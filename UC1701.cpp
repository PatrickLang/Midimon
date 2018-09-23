#include "UC1701.h"

#include <avr/pgmspace.h>

#include <SPI.h>
#include <stdint.h>

static uint8_t g_ss;
static uint8_t g_cd;
static uint8_t g_reset;
static uint8_t g_scroll;

static const SPISettings g_spiSettings(10000000, MSBFIRST, SPI_MODE0);

class Transaction
{
public:
	inline Transaction()
	{
		digitalWrite(g_ss, LOW);
		SPI.beginTransaction(g_spiSettings);
	}

	inline ~Transaction()
	{
		digitalWrite(g_ss, HIGH);
		SPI.endTransaction();
	}
};

void uc1701_init(int ss, int cd, int reset)
{
	g_ss = ss;
	g_cd = cd;
	g_reset = reset;
	g_scroll = 0;

	pinMode(g_ss, OUTPUT);
	pinMode(g_cd, OUTPUT);
	pinMode(g_reset, OUTPUT);

	SPI.begin();

	digitalWrite(g_reset, LOW);

	uc1701_reset();
}

enum Mode
{
	COMMAND,
	DATA
};

inline static void uc1701_mode(Mode mode)
{
	digitalWrite(g_cd, mode == DATA);
}

inline static void uc1701_send(uint8_t byte)
{
	SPI.transfer(byte);
}

inline static void uc1701_set_power_control(bool boost_on, bool vreg_on, bool vfol_on)
{
	uc1701_send(0x28 | (boost_on << 2) | (vreg_on << 1) | (vfol_on << 0));
}

inline static void uc1701_set_scroll_line(uint8_t line)
{
	uc1701_send(0x40 | (line & 0x3f));
}

inline static void uc1701_set_column_address(uint8_t address)
{
	uc1701_send(address & 0x0f);
	uc1701_send(0x10 | (address >> 4));
}

inline static void uc1701_set_page_address(uint8_t address)
{
	uc1701_send(0xb0  | (address & 0x07)); // Actual range is 0-8, but 0-7 should be enough.
}

inline static void uc1701_set_resistor_ratio(uint8_t ratio)
{
	uc1701_send(0x20 | (ratio & 0x07));
}

inline static void uc1701_set_electronic_volume(uint8_t volume) // A.K.A. PM 
{
	uc1701_send(0x81);
	uc1701_send(volume & 0x3f);
}

inline static void uc1701_set_all_pixels_on(bool on)
{
	uc1701_send(0xa4 | (on ? 1 : 0));
}

inline static void uc1701_set_inverse_display(bool on)
{
	uc1701_send(0xa6 | (on ? 1 : 0));
}

inline static void uc1701_set_display_enable(bool on)
{
	uc1701_send(0xae | (on ? 1 : 0));
}

inline static void uc1701_set_mirror_x(bool on)
{
	uc1701_send(0xa0 | (on ? 1 : 0));
}

inline static void uc1701_set_mirror_y(bool on)
{
	uc1701_send(0xc0 | (on ? 0x08 : 0));
}

inline static void uc1701_sys_reset()
{
	uc1701_send(0xe2);
}

inline static void uc1701_nop()
{
	uc1701_send(0xe3);
}

inline static void uc1701_set_bias_ratio(uint8_t bias)
{
	uc1701_send(0xa2 | (bias ? 1 : 0));
}

inline static void uc1701_set_booster_ratio(uint8_t ratio)
{
	uc1701_send(0xf8);
	uc1701_send(ratio & 3);
}

inline static void uc1701_set_static_indicator_on(bool on)
{
	if (on)
	{
		uc1701_send(0xad);
		uc1701_send(0xe3);
	}
	else
	{
		uc1701_send(0xac);
	}
}

inline static void uc1701_set_advanced_settings(bool tc, bool hwrap, bool vwrap)
{
	uc1701_send(0xfa);
	uc1701_send((tc << 7) | (hwrap << 1) | (vwrap << 0));
}

void uc1701_reset()
{
	digitalWrite(g_reset, LOW);
	delay(1);
	digitalWrite(g_reset, HIGH);
	delay(5);

	g_scroll = 0;

	{
		Transaction t;

		uc1701_mode(COMMAND);
		uc1701_set_display_enable(false);               // 0xae
		uc1701_set_scroll_line(0);                      // 0x40
		uc1701_set_page_address(0);                     // 0xb0
		uc1701_set_column_address(0);                   // 0x00 0x10
		uc1701_set_mirror_x(false);                     // 0xa0
		uc1701_set_mirror_y(false);                     // 0xc0
		uc1701_set_inverse_display(false);              // 0xa6
		uc1701_set_bias_ratio(0);                       // 0xa2
		uc1701_set_power_control(true, true, true);     // 0x2f
		uc1701_set_booster_ratio(0x00);                 // 0xf8 0x00
		uc1701_set_resistor_ratio(3);                   // 0x23
		uc1701_set_electronic_volume(0x27);             // 0x81 0x27
		uc1701_set_static_indicator_on(false);          // 0xac
		uc1701_set_display_enable(false);               // 0xae
		uc1701_set_all_pixels_on(true);                 // 0xa5
		uc1701_set_advanced_settings(true, true, true); // 0xfa 0x83

		uc1701_set_all_pixels_on(false);                // 0xa4
		uc1701_set_display_enable(true);                // 0xaf

		uc1701_clear();

#if 0
		uc1701_mode(COMMAND);
		uc1701_set_column_address(0);
		uc1701_set_page_address(64 / 8 - 1);
		uc1701_mode(DATA);
		for (int i=0; i<64; ++i)
		{
			uc1701_send(0xaa);
		}
		for (int i=64; i<128; ++i)
		{
			uc1701_send(0x55);
		}
		uc1701_mode(COMMAND);
		uc1701_set_scroll_line(32);
		uc1701_set_column_address(0);
		uc1701_set_page_address(0);
		uc1701_mode(DATA);

		for (int i = 0; i<64; ++i)
		{
			uc1701_send(0xaa);
		}
		for (int i = 64; i<128; ++i)
		{
			uc1701_send(0x55);
		}
#endif
	}
}

void uc1701_clear()
{
	uc1701_mode(DATA);
	Transaction t;
	for (int i = 0; i<132 * 65 / 8; ++i)
	{
		uc1701_send(0);
		uc1701_send(0);
	}
}

void uc1701_set_scroll(uint8_t line)
{
	g_scroll = line & 0x3f;
	uc1701_mode(COMMAND);
	Transaction t;
	uc1701_set_scroll_line(g_scroll);
}

void uc1701_add_scroll(int8_t delta)
{
	uc1701_set_scroll(g_scroll + delta);
}

void uc1701_set_position(uint8_t x, uint8_t y)
{
	uc1701_mode(COMMAND);
	Transaction t;
	uc1701_set_column_address(x);
	uc1701_set_page_address((g_scroll >> 3) + y);
}

void uc1701_draw_space(uint8_t n, bool inverse)
{
	uc1701_mode(DATA);
	Transaction t;
	uint8_t byte = !inverse ? 0x00 : 0xff;
	while (n-- != 0)
		uc1701_send(byte);
}

void uc1701_draw_bitmap(const void *data, uint8_t n, bool inverse)
{
	uc1701_mode(DATA);
	Transaction t;
	const uint8_t *p = (const uint8_t*)data;
	if (!inverse)
	{
		while (n-- != 0)
			uc1701_send(*p++);
	}
	else
	{
		while (n-- != 0)
			uc1701_send(~(*p++));
	}
}

void uc1701_draw_progmem_bitmap(const void *data, uint8_t n, bool inverse)
{
	uc1701_mode(DATA);
	Transaction t;
	const uint8_t *p = (const uint8_t *)data;
	if (!inverse)
	{
		while (n-- != 0)
		{
			uint8_t byte = pgm_read_byte(p++);
			uc1701_send(byte);
		}
	}
	else
	{
		while (n-- != 0)
		{
			uint8_t byte = pgm_read_byte(p++);
			uc1701_send(~byte);
		}
	}
}
