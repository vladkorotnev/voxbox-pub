#include <util.h>
#include <settings.h>
#include <EEPROMWearLevel.h>

#define SETTINGS_VERSION 0

void cfg_begin() {
    EEPROMwl.begin(SETTINGS_VERSION, cfg_index::MAX_INVALID);
}

int cfg_read(cfg_index_t idx) {
    int tmp = -1;
    EEPROMwl.get(idx, tmp);
    dbg_log(F("[CFG] READ [%i] => %i"), idx, tmp);
    return tmp;
} 

void cfg_write(cfg_index_t idx, int value) {
    inf_log(F("[CFG] SET [%i] = [%i]"), idx, value);
    EEPROMwl.put(idx, value);
}
