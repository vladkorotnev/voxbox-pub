#ifndef SETTINGS_H_
#define SETTINGS_H_
#include <util.h>

void cfg_begin();

typedef enum cfg_index {
    STARTUP_ROM_MODE = 0,
    MAX_INVALID
} cfg_index_t;

int cfg_read(cfg_index_t);
void cfg_write(cfg_index_t, int value);

#endif