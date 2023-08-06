#ifndef LYRIC_H_
#define LYRIC_H_
#include <stddef.h>

void lyric_begin();
void parse_lyric_at_once(const char *);
void lyric_get_past(char * buf, size_t count);
void lyric_get_future(char * buf, size_t count);
void lyric_advance();
void lyric_clear();

#endif