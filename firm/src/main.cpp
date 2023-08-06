#include <log.h>
#include <disp.h>
#include <midiif.h>
#include <evy_ctl.h>
#include <lyric.h>
#include <settings.h>
#include <util.h>

extern "C" {
  void setup();
  void loop();
}

void setup() {
  // Bootstrap
  log_begin();
  inf_log(F("Starting SAS-DOS..."));

  // Set up hardware
  disp_begin();
  evy_ctl_begin();

  // Set up software
  cfg_begin();
  lyric_begin();
  midi_begin();

  // Load settings
  int default_rom = cfg_read(cfg_index_t::STARTUP_ROM_MODE);
  if(default_rom == rom_selection_t::MAIN_ROM || default_rom == rom_selection_t::SUB_ROM) {
    evy_select_rom((rom_selection_t) default_rom);
  }

  // Begin operation
  inf_log(F("Setup complete, release the kraken!"));
  evy_reset();
}


void loop() {
  midi_tick();
  disp_tick();
}
