#include "evy_ctl.h"

#define EVY_SET_RST(x) digitalWrite(EVY_RST_PIN, x)

static rom_selection_t _cur_rom = MAIN_ROM;

void _evy_rom_cs_out_internal(rom_selection_t rom) {
    if(rom == SUB_ROM) {
        inf_log(F("[EVY] bukva: SUS"));
        digitalWrite(EVY_SUB_ROM_SEL_PIN, LOW);
    }
    else {
        inf_log(F("[EVY] bukva: NUS"));
        digitalWrite(EVY_SUB_ROM_SEL_PIN, HIGH);
    }

    _cur_rom = rom;
}

rom_selection_t evy_current_rom() {
    return _cur_rom;
}

void evy_ctl_begin() {
    inf_log(F("[EVY] VSTUTZ!"));
    pinMode(EVY_RST_PIN, OUTPUT);
    pinMode(EVY_SUB_ROM_SEL_PIN, OUTPUT);
    EVY_SET_RST(LOW); // keep eVY inactive until further notice
    _evy_rom_cs_out_internal(MAIN_ROM); // sensible default
}

void evy_reset() {
    inf_log(F("[EVY] ALL SAS OELIUTZ!"));
    EVY_SET_RST(LOW);
    delayMicroseconds(500);
    EVY_SET_RST(HIGH);
}

void evy_select_rom(rom_selection_t rom) {
    // NB: use the F0 43 79 09 01 01 00 F7 sysex instead 
    _evy_rom_cs_out_internal(rom);
}
