#ifndef UTIL_H_
#define UTIL_H_

#include <log.h>

#define __UNUSED_ARG(x) (void) x;

typedef enum rom_selection {
    MAIN_ROM = 0,
    SUB_ROM = 1
} rom_selection_t;

#endif