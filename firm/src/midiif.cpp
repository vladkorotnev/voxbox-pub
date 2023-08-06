#include <MIDI.h>
#include <util.h>
#include <evy_ctl.h>
#include <midiif.h>
#include <disp.h>
#include <lyric.h>
#include <settings.h>

struct MySettings : public midi::DefaultSettings
{
    static const unsigned SysExMaxSize = MIDIIF_MAX_SYSEX;
    static const unsigned BaudRate = MIDIIF_BAUD;
    static const bool Use1ByteParsing = false;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI, MySettings);

void handle_lyric_set(byte* array, unsigned size) {
    inf_log(F("Got lyric line: %s"), array);
    parse_lyric_at_once(array);
}

inline void handle_3rdparty_sysex(byte* array, unsigned size) {
    if(size >= 3 && array[1] == 0x43) {
        // Yamaha related
        if(size >= 5 && array[2] == 0x79 && array[3] == 0x09) {
            // eVY related
            if(size > 8 && array[4] == 0x00 && array[5] == 0x50 && array[6] == 0x10) {
                // lyrics command
                handle_lyric_set(&array[7], size - 8);
                disp_midi_light();
            } else if (size == 8 && array[4] == 0x01 && array[5] == 0x01 && array[6] == 0x00) {
                // reset command
                disp_show_msg(500, "  RESET DSP !   ", nullptr);
                lyric_clear();
                evy_reset();
            }
        }
    }
}

void midi_on_sysex(byte* array, unsigned size) {
    #ifdef MIDI_DUMP
    for(int i = 0; i<size; i++) {
        if (array[i] < 16) {Serial.print("0");}
        Serial.print(array[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
    #endif

    if(size < 3) return; // too short!
    if(array[1] == MIDIIF_SYSEX_DSP_MANUF_ID) {
        handle_3rdparty_sysex(array, size);
    } else if(array[1] == MIDIIF_SYSEX_SV_MANUF_ID) {
        if(size < 4) return;
        switch(array[2]) {
            case MIDIIF_SV_CMD_RST:
                disp_show_msg(500, "  RESET DSP !!  ", nullptr);
                lyric_clear();
                evy_reset();
                break;
            
            case MIDIIF_SV_CMD_ROM_SEL:
                {
                    if(size < 5) {
                        err_log(F("[MIDI] ROM sel size low"));
                        disp_show_msg(500, " Illegal Data!! ", nullptr);
                        return;
                    }
                    rom_selection_t rom;
                    if(array[3] == 0x0) {
                        rom = MAIN_ROM;
                    } else if (array[3] == 0x1) {
                        rom = SUB_ROM;
                    } else {
                        disp_show_msg(500, " Sel \x01 no. bad!", nullptr);
                        err_log(F("[MIDI] ROM sel arg bad"));
                        return;
                    }
                    if(evy_current_rom() != rom) {
                        evy_select_rom(rom);
                        lyric_clear();
                    }
                }
                break;

            case MIDIIF_SV_CMD_ROM_SAVE:
                {
                    rom_selection_t cur_rom = evy_current_rom();
                    cfg_write(cfg_index_t::STARTUP_ROM_MODE, (int) cur_rom);
                }
                break;

            case MIDIIF_SV_CMD_MSG:
                if(size < 6) {
                    disp_stop_msg();
                    err_log(F("[MIDI] MSG size low"));
                    return;
                }
                if((array[4] == 0x0 || array[3] == 0x0) && size == 5) {
                    // empty message or 0 time cancels display
                    disp_stop_msg();
                } else {
                    uint8_t time = array[3];
                    char * msg_ptr = (char*) &array[4];
                    char * line1 = nullptr;
                    char * line2 = nullptr;

                    line1 = msg_ptr;
                    msg_ptr += strlen(line1);
                    msg_ptr++;
                    if(*msg_ptr != 0xF7) {
                        line2 = msg_ptr;
                    }
                    disp_show_msg(time * 1000, line1, line2);
                }
                break;

            default:
                disp_show_msg(500, " Illegal Cmd !! ", nullptr);
                err_log(F("[MIDI] Unknown sysex cmd=0x%x"), array[2]);
                return;
        }

        disp_midi_light();
    }
}

byte now_sing_note = 0;
void midi_on_noteon(byte channel, byte note, byte velocity) {
    if(channel == 1) {
        lyric_advance();
        disp_midi_light();
        disp_midi_sing(true);
        now_sing_note = note;
    }
}

void midi_on_noteoff(byte channel, byte note, byte velocity) {
    if(channel == 1) {
        disp_midi_light();
        if(note == now_sing_note) {
            disp_midi_sing(false);
            disp_midi_light();
            now_sing_note = 0;
        }
    }
}

void midi_on_reset() {
    inf_log(F("[MIDI] Reset..."));
}

void midi_tick() {
    MIDI.read();
}

void midi_begin() {
    pinMode(13, OUTPUT);
    inf_log(F("[MIDI] I/F start"));
    MIDI.begin();
    MIDI.turnThruOff();
    MIDI.setHandleSystemExclusive(midi_on_sysex);
    MIDI.setHandleSystemReset(midi_on_reset);
    MIDI.setHandleNoteOn(midi_on_noteon);
    MIDI.setHandleNoteOff(midi_on_noteoff);
}