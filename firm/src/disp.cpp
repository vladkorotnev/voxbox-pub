#include <Arduino.h>
#include <AsyncCrystal_I2C.h>
#include <util.h>
#include <disp.h>
#include <evy_ctl.h>
#include <lyric.h>

AsyncCrystal_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according

const char CHAR_ICON_ROM = 1;
PROGMEM const char ICON_ROM[] = {
    0b01110,
    0b11111,
    0b01110,
    0b11111,
    0b01110,
    0b11111,
    0b01110,
    0b00000,
};

PROGMEM const char MAIN_ROM_NAME[] = " VY1";
PROGMEM const char SUB_ROM_NAME[] = "MIKU";

const char CHAR_SING_ON = 0xEF;
const char CHAR_SING_OFF = 0x8A;
const char CHAR_NOTE_ON = 0x19;
const char CHAR_NOTE_OFF = ' ';

#ifdef ASYNCCRYSTAL_VFD
const char CHAR_PHONEME_CURSOR = ' ';
#else
const char CHAR_PHONEME_CURSOR = '|';
#endif

const char CHAR_SPINNER = 7;
const size_t SPINNER_PHASES_MAX = 7;
static int spinner_phase = 0;
static int old_spinner_phase = 0;
PROGMEM const char ICON_SPINNER_FRAMES[] = { 
    0b00000,
    0b00011,
    0b00001,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,

    0b00000,
    0b00000,
    0b00001,
    0b00001,
    0b00001,
    0b00000,
    0b00000,
    0b00000,

    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00001,
    0b00011,
    0b00000,
    0b00000,

    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b00000,
    0b00000,

    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b10000,
    0b11000,
    0b00000,
    0b00000,

    0b00000,
    0b00000,
    0b10000,
    0b10000,
    0b10000,
    0b00000,
    0b00000,
    0b00000,

    0b00000,
    0b11000,
    0b10000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,

    0b00000,
    0b01110,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,

 };


static uint16_t msg_dur = 0;
static bool msg_top_only = false;
unsigned long last_millis = 0;
static uint8_t singing_sts = 0;
static bool need_clear = false;

void disp_spinner_update(bool sync) {
    if(spinner_phase > SPINNER_PHASES_MAX) spinner_phase = 0;
    lcd.createChar(CHAR_SPINNER, &ICON_SPINNER_FRAMES[spinner_phase * 8]);
    if(sync) lcd.flush();
    old_spinner_phase = spinner_phase;
}

void disp_sus_upd_ms(uint16_t time) {
    msg_dur = time / DISP_UPD_INTERVAL;
}

void disp_show_msg(uint16_t time, const char * top_line, const char * bottom_line) {
    lcd.setCursor(0, 0);
    lcd.print(top_line);
    if(bottom_line == nullptr) {
        msg_top_only = true;
    } else {
        msg_top_only = false;
        lcd.setCursor(0, 1);
        lcd.print(bottom_line);
    }
    disp_sus_upd_ms(time);
    need_clear = true;
}

void disp_stop_msg() {
    disp_sus_upd_ms(0);
}

void disp_intro() {
    lcd.setCursor(0, 0);
    lcd.print(F("Genjitsu Labs    "));
    lcd.flush();
    lcd.setCursor(0, 1);
    lcd.print(F("     VoxBox v1.0"));
    lcd.flush();
    msg_top_only = false;
    need_clear = true;
    disp_sus_upd_ms(1600);
}

void disp_midi_light() {
    if(old_spinner_phase != spinner_phase) return; // too early to change, the old one wasn't shown yet

    if(spinner_phase == SPINNER_PHASES_MAX) spinner_phase = 0;
    else spinner_phase++;
}

void disp_draw_home_top() {
    char lyric_line[17] = { 0 };
    lyric_get_past(&lyric_line[0], 7);
    lyric_line[7] = CHAR_PHONEME_CURSOR;
    lyric_get_future(&lyric_line[8], 8);
    lcd.setCursor(0, 0);
    lcd.print(lyric_line);
}

void disp_draw_home_bottom() {
    lcd.setCursor(0,1);
    lcd.write(CHAR_ICON_ROM);
    lcd.printProgMem(evy_current_rom() == MAIN_ROM ? MAIN_ROM_NAME : SUB_ROM_NAME);

    lcd.setCursor(12,1);
    if(singing_sts > 0) {
        lcd.write(CHAR_NOTE_ON);
        lcd.write(CHAR_SING_ON);
    } else {
        lcd.write(CHAR_NOTE_OFF);
        lcd.write(CHAR_SING_OFF);
    }
    lcd.write(' ');
    lcd.write(CHAR_SPINNER);
}

void disp_midi_sing(bool sing) {
    singing_sts = sing ? 2 : 0;
}

void disp_tick() {
    unsigned long time = millis();
   if(!lcd.busy() && time != last_millis && time % 8 == 0)  {
        if(msg_dur == 0) {
            if(need_clear) {
                lcd.clear();
                need_clear = false;
            }

            disp_spinner_update(false);
            disp_draw_home_top();
            disp_draw_home_bottom();
        } else {
            msg_dur--;
            if(msg_top_only) {
                disp_spinner_update(false);
                disp_draw_home_bottom();
            }
        }

        last_millis = time;
   }

   lcd.loop();
}

void disp_begin() {
    inf_log(F("[LCD] Begin"));
    lcd.init();
    #ifdef ASYNCCRYSTAL_VFD
        lcd.vfd_brightness(BRIGHT_50); // seems to be enough to be super bright as is
    #endif
    disp_intro();

    lcd.createChar(CHAR_ICON_ROM, ICON_ROM);
    lcd.flush();
    disp_spinner_update(true); // prime the first frame

    lcd.backlight();
    lcd.noCursor();
    lcd.flush();
}