#include "midimon_renderers.h"

EventRenderer::EventRenderer()
{
}

void EventRenderer::begin(IMidimonDisplay & display)
{
}

void EventRenderer::handleInputEvent(MidimonPort src, const midi_event_t & event)
{
}

void EventRenderer::handleOutputEvent(MidimonPort dst, const midi_event_t & event)
{
}
