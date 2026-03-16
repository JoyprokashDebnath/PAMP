#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "../state/state.h"
#include "../audio/player.h"

void playlist_next(AppState *s, Player *p);
void playlist_prev(AppState *s, Player *p);
void playlist_play_index(AppState *s, Player *p, int index);

#endif
