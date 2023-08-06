#ifndef MIDI_H_
#define MIDI_H_
#include <util.h>

#define MIDIIF_MAX_SYSEX 512
#define MIDIIF_BAUD 31250

#define MIDIIF_SYSEX_SV_MANUF_ID 0x7B // our own manufacturer id
#define MIDIIF_SYSEX_DSP_MANUF_ID 0x43 // manufacturer id of the DSP we're watching

#define MIDIIF_SV_CMD_RST 0x7F
#define MIDIIF_SV_CMD_ROM_SEL 0x00
#define MIDIIF_SV_CMD_ROM_SAVE 0x01
#define MIDIIF_SV_CMD_MSG 0x02

void midi_begin();
void midi_tick();

#endif