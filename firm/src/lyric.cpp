#include <lyric.h>
#include <util.h>
#include <HashMap.h>

const size_t token_buffer_count = 511;
char token_buffer[token_buffer_count + 1] = { 0 };
size_t token_buffer_end = 0;
size_t token_buffer_speak_cursor = 0;

void lyric_begin() {
}

void lyric_clear() {
    token_buffer_end = 0;
    token_buffer[0] = 0x0;
}

void parse_lyric_at_once(const char * lyric_set) {
    inf_log(F("[LYR] Parse start"));
    int i = 0;
    char c = lyric_set[i];
    size_t token_buffer_write_idx = 0;
    while (c != 0x0 && i < token_buffer_count) {
        if(token_buffer_end == token_buffer_count) {
            err_log(F("[LYR] Token buffer overflow!"));
            break;
        }

        if(c == ' ') {
            // no op
        } else if(c == ',') {
            token_buffer[token_buffer_write_idx++] = ' ';
        } else {
            token_buffer[token_buffer_write_idx++] = c;
        }
        i++;
        c = lyric_set[i];
    }

    token_buffer[token_buffer_write_idx] = 0x0;
    token_buffer_end = token_buffer_write_idx;
    token_buffer_speak_cursor = 0;
    inf_log(F("Lyric parsed: %s, end=%i"), token_buffer, token_buffer_end);
}

void lyric_get_past(char * buf, size_t count) {
    memset((void*) buf, ' ', count);

    size_t i = token_buffer_speak_cursor;
    size_t x = 0;
    if(i == 0) return;
    i--; // now i points to a space
    
    for(x = 0; x < count; x++) {
        buf[count - x] = token_buffer[i];
        i--;
        if(i == 0) break;
    }
    if(i == 0 && x < count) {
        x++;
        buf[count - x] = token_buffer[i];
    }
    //dbg_log(F("get_past: [%s]"), buf);
}

void lyric_get_future(char * buf, size_t count) {
    size_t i = 0;
    for(i = 0; i < count; i++) {
        size_t idx = token_buffer_speak_cursor + i;
        if(token_buffer_end <= idx) {
            break;
        }
        buf[i] = token_buffer[idx];
    }

    for(; i < count; i++) buf[i] = ' ';
    //dbg_log(F("get_future: [%s]"), buf);
}

void lyric_advance() {
    size_t i = token_buffer_speak_cursor;
    char c = token_buffer[i];
    while(i < token_buffer_end && c != ' ')  c = token_buffer[i++];
    
    if(i == token_buffer_end) i = 0;

    token_buffer_speak_cursor = i;
}