#include "midimon_event_display_mode.h"

#include "midimon_display.h"
#include "midimon_renderer.h"

#include <midi_serialization.h>
#include <midi_messages.h>

enum StringId
{
	STRING_POLY_AFTERTOUCH = 0,
	STRING_CHANNEL_AFTERTOUCH,
	STRING_PITCH_BEND,
	STRING_TUNE_REQUEST,
	STRING_MIDI_TIME_CODE,
	STRING_SONG_SELECT,
	STRING_SONG_POSITION_POINTER,
	STRING_CLOCK,
	STRING_START,
	STRING_CONTINUE,
	STRING_STOP,
	STRING_ACTIVE_SENSE,
	STRING_RESET,
	STRING_INPUT,
	STRING_OUTPUT,
	STRING_PROGRAM_CHANGE,
	STRING_SYSEX,

	// Must be the last one!
	STRING_COUNT
};

static const char STR_POLY_AFTERTOUCH[]       PROGMEM = "Aft.";
static const char STR_CHANNEL_AFTERTOUCH[]    PROGMEM = "Ch.Aft.";
static const char STR_PITCH_BEND[]            PROGMEM = "P.Bnd. ";
static const char STR_TUNE_REQUEST[]          PROGMEM = "Tune req.";
static const char STR_MIDI_TIME_CODE[]        PROGMEM = "MTC";
static const char STR_SONG_SELECT[]           PROGMEM = "SongSel.";
static const char STR_SONG_POSITION_POINTER[] PROGMEM = "SPP";
static const char STR_CLOCK[]                 PROGMEM = "Clock";
static const char STR_START[]                 PROGMEM = "Start";
static const char STR_CONTINUE[]              PROGMEM = "Continue";
static const char STR_STOP[]                  PROGMEM = "Stop";
static const char STR_ACTIVE_SENSE[]          PROGMEM = "Active Sense";
static const char STR_RESET[]                 PROGMEM = "Reset";
static const char STR_INPUT[]                 PROGMEM = "Input";
static const char STR_OUTPUT[]                PROGMEM = "Output";
static const char STR_PROGRAM_CHANGE[]        PROGMEM = "PgmCh.";
static const char STR_SYSEX[]                 PROGMEM = "SX";

static PGM_P const g_stringTable[STRING_COUNT] PROGMEM =
{
	STR_POLY_AFTERTOUCH,
	STR_CHANNEL_AFTERTOUCH,
	STR_PITCH_BEND,
	STR_TUNE_REQUEST,
	STR_MIDI_TIME_CODE,
	STR_SONG_SELECT,
	STR_SONG_POSITION_POINTER,
	STR_CLOCK,
	STR_START,
	STR_CONTINUE,
	STR_STOP,
	STR_ACTIVE_SENSE,
	STR_RESET,
	STR_INPUT,
	STR_OUTPUT,
	STR_PROGRAM_CHANGE,
	STR_SYSEX,
};

static const char *getString_P(StringId str)
{
	if (str < 0 || str >= STRING_COUNT)
		return NULL;

	return (PGM_P)pgm_read_word(&(g_stringTable[str]));
}

EventDisplayMode::EventDisplayMode()
	:m_x(0)
{
}

void EventDisplayMode::onInit()
{
	MidimonSettings::registerListener(*this);
}

void EventDisplayMode::onEnter(Midimon *midimon)
{
	MidimonModeBase::onEnter(midimon);

	clear();
}

void EventDisplayMode::onBackPressed()
{
	if (getMidimon())
		clear();
}

void EventDisplayMode::clear()
{
	m_x = 0;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	renderer.clear();

	for (int i = 0; i<8; ++i)
		newLine();

	renderer.setDrawPosition(21, 7);
	renderer.printString_P(getString_P(STRING_INPUT));

	renderer.setDrawPosition(86, 7);
	renderer.printString_P(getString_P(STRING_OUTPUT));
}

void EventDisplayMode::printEvent(MidimonRenderer &renderer, const midi_event_t &event)
{
	if (!m_decodeBytes)
	{
		printRaw(renderer, event);
		fillRemainingPixels(renderer);
		return;
	}

	uint8_t eventId = event.m_event & 0x0f;
	switch (eventId)
	{
	case 0x08:
	case 0x09:
		printNote(renderer, event.m_data[0] & 0xf, event.m_data[1], event.m_data[2], eventId == 0x09);
		break;
	case 0x0b:
		printCC(renderer, event.m_data[0] & 0xf, event.m_data[1], event.m_data[2]);
		break;
	case 0x0c:
		printProgChange(renderer, event.m_data[0] & 0xf, event.m_data[1]);
		break;
	case 0x05:
		if (midi_is_single_byte_system_common(event.m_data[0]))
		{
			printSystemCommon(renderer, event, 1);
			break;
		}
		// Fallthrough intentional.
	case 0x04:
	case 0x06:
	case 0x07:
		printSysex(renderer, event.m_data, (eventId + 1) % 3 + 1);
		break;
	case 0x02: // 2 byte system common.
	case 0x03: // 3 byte system common.
		printSystemCommon(renderer, event, eventId);
		break;
	case 0x0f: // Single byte.
		printSystemRealtime(renderer, event);
		break;
	case 0x0a: // Aftertouch.
		printPolyAftertouch(renderer, event.m_data[0] & 0xf, event.m_data[1], event.m_data[2]);
		break;
	case 0x0d: // Channel pressure.
		printAftertouch(renderer, event.m_data[0] & 0xf, event.m_data[1]);
		break;
	case 0x0e: // Pitch bend.
		printPitchBend(renderer, event.m_data[0] & 0xf, (((uint16_t)event.m_data[2]&0x7f) << 7) | (event.m_data[1]&0x7f));
		break;
	default:
		m_x += renderer.printMidiEventHex(event);
		break;
	}

	fillRemainingPixels(renderer);
}

void EventDisplayMode::printNote(MidimonRenderer &renderer, uint8_t ch, uint8_t note, uint8_t vel, bool on)
{
	m_x += renderer.printSymbol(on ? SYM_NOTE : SYM_REST);
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printNote(note);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(vel, 3);
}

void EventDisplayMode::printPolyAftertouch(MidimonRenderer & renderer, uint8_t ch, uint8_t note, uint8_t pressure)
{
	m_x += renderer.printString_P(getString_P(STRING_POLY_AFTERTOUCH));
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printNote(note);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(pressure, 3);
}

void EventDisplayMode::printAftertouch(MidimonRenderer & renderer, uint8_t ch, uint8_t pressure)
{
	m_x += renderer.printString_P(getString_P(STRING_CHANNEL_AFTERTOUCH));
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(pressure, 3);
}

void EventDisplayMode::printPitchBend(MidimonRenderer & renderer, uint8_t ch, uint16_t value)
{
	m_x += renderer.printString_P(getString_P(STRING_PITCH_BEND));
	m_x += renderer.printDec(ch + 1, 1);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec16((int16_t)(value - 0x2000));
}

void EventDisplayMode::printCC(MidimonRenderer & renderer, uint8_t ch, uint8_t cc, uint8_t val)
{
	m_x += renderer.printSymbol(SYM_CONTINUOUS_CONTROLLER);
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(cc, 4);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(val, 3);
}

void EventDisplayMode::printProgChange(MidimonRenderer & renderer, uint8_t ch, uint8_t prog)
{
	m_x += renderer.printString_P(getString_P(STRING_PROGRAM_CHANGE));
	m_x += renderer.printDec(ch + 1, 2);
	m_x += renderer.printChar(' ');
	m_x += renderer.printDec(prog, 3);
}

void EventDisplayMode::printSysex(MidimonRenderer & renderer, const uint8_t * data, uint8_t n)
{
	m_x += renderer.printString_P(getString_P(STRING_SYSEX));
	m_x += renderer.printChar(' ');
	for (uint8_t i = 0; i<n; ++i)
	{
		m_x += renderer.printHex(data[i]);
		m_x += renderer.printChar(' ');
	}
}

void EventDisplayMode::printRaw(MidimonRenderer &renderer, const midi_event_t &event)
{
	m_x += renderer.printMidiEventHex(event);
}

void EventDisplayMode::printSystemCommon(MidimonRenderer &renderer, const midi_event_t &event, uint8_t len)
{
	switch (len)
	{
	case 1:
		if (event.m_data[0] == 0xf6)
		{
			m_x += renderer.printString_P(getString_P(STRING_TUNE_REQUEST));
			return;
		}
		break;
	case 2:
		if (event.m_data[0] == 0xf1)
		{
			m_x += renderer.printString_P(getString_P(STRING_MIDI_TIME_CODE));
			m_x += renderer.printDec((event.m_data[1] >> 4) & 7, 2);
			m_x += renderer.printDec((event.m_data[1] & 0xf), 3);
			return;
		}
		else if (event.m_data[0] == 0xf3)
		{
			m_x += renderer.printString_P(getString_P(STRING_SONG_SELECT));
			m_x += renderer.printDec(event.m_data[1] & 0x7f, 4);
			return;
		}
		break;
	case 3:
		if (event.m_data[0] == 0xf2)
		{
			m_x += renderer.printString_P(getString_P(STRING_SONG_POSITION_POINTER));
			uint16_t position = (((uint16_t)event.m_data[2]&0x7f) << 7) | (event.m_data[1]&0x7f);
			uint16_t bar = position / 16 + 1;
			uint16_t measure = ((position % 16) / 4) + 1;
			m_x += renderer.printDec16(bar, 3);
			m_x += renderer.printChar(':');
			m_x += renderer.printChar(measure + '0');
			return;
		}
	default:
		break;
	}

	m_x += renderer.printMidiEventHex(event);
}

void EventDisplayMode::printSystemRealtime(MidimonRenderer & renderer, const midi_event_t &event)
{
	uint8_t byte = event.m_data[0];

	switch (byte)
	{
	case 0xf8: // Clock.
		m_x += renderer.printString_P(getString_P(STRING_CLOCK));
		return;
	case 0xfa: // Start.
		m_x += renderer.printString_P(getString_P(STRING_START));
		return;
	case 0xfb: // Continue.
		m_x += renderer.printString_P(getString_P(STRING_CONTINUE));
		return;
	case 0xfc: // Stop.
		m_x += renderer.printString_P(getString_P(STRING_STOP));
		return;
	case 0xfe: // Active sensing.
		m_x += renderer.printString_P(getString_P(STRING_ACTIVE_SENSE));
		return;
	case 0xff: // Reset.
		m_x += renderer.printString_P(getString_P(STRING_RESET));
		return;
	default:
		break;
	}

	m_x += renderer.printMidiEventHex(event);
}

void EventDisplayMode::fillRemainingPixels(MidimonRenderer &renderer)
{
	uint32_t empty = 0;
	uint8_t n = (m_x < 63 ? 63 : 128) - m_x;
	uint8_t i;
	for (i=0; i+4<n; i+=4)
		renderer.drawBitmap(&empty, 4);
	for (; i<n; ++i)
		renderer.drawBitmap(&empty, 1);
}

void EventDisplayMode::onChange(SettingId settingId, SettingValueType value)
{
	bool handled = false;

	switch (settingId)
	{
	case SETTING_DECODE_HEX:
		handled = true;
		m_decodeBytes = value != 0;
		break;
	case SETTING_FILTER_NOISY_MSG:
		handled = true;
		m_filterNoisyMsg = value != 0;
		break;
	}

	if (handled && getMidimon())
		clear();
}

bool EventDisplayMode::shouldFilterOut(const midi_event_t &event) const
{
	uint8_t byte = event.m_data[0];

	// MIDI Sync
	if (byte == 0xf8)
		return true;

	// Active Sensing
	if (byte == 0xfe)
		return true;

	return false;
}

void EventDisplayMode::onIncomingMidiEvent(MidimonPort src, const midi_event_t &event)
{
	if (m_filterNoisyMsg && shouldFilterOut(event))
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	newLine();
	m_x = 0;
	renderer.setDrawPosition(m_x, 0);
	printEvent(renderer, event);
}

void EventDisplayMode::onOutgoingMidiEvent(MidimonPort dst, const midi_event_t &event)
{
	if (m_filterNoisyMsg && shouldFilterOut(event))
		return;

	MidimonRenderer &renderer = getMidimon()->getRenderer();

	m_x = 65;
	renderer.setDrawPosition(m_x, 0);
	printEvent(renderer, event);
}

void EventDisplayMode::newLine()
{
	MidimonRenderer &renderer = getMidimon()->getRenderer();

	uint8_t vline = 0xff;
	renderer.addVerticalScroll(-8);
	renderer.setDrawPosition(63, 0);
	renderer.drawBitmap(&vline, sizeof(vline));
}
