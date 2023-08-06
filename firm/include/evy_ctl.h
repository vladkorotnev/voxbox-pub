#ifndef EVY_CTL_H_
#define EVY_CTL_H_

#include "util.h"

#ifndef EVY_RST_PIN
#define EVY_RST_PIN 4
#endif

#ifndef EVY_SUB_ROM_SEL_PIN
#define EVY_SUB_ROM_SEL_PIN 8
#endif

void evy_ctl_begin();
void evy_reset();
void evy_select_rom(rom_selection_t sub);
rom_selection_t evy_current_rom();

#endif