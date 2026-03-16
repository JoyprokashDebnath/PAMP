#ifndef SCANNER_H
#define SCANNER_H

#include "../state/state.h"

/* Recursively scan path for audio files.
 F il*ls state->tracks[] and sets state->track_count.
 Returns number of tracks found. */
int scan_library(AppState *s, const char *path);

#endif
