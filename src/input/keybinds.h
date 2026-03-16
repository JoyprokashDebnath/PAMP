#ifndef KEYBINDS_H
#define KEYBINDS_H

#include "../state/state.h"
#include "../audio/player.h"

void handle_input(AppState *s, Player *p, int ch);

/* Called by main when MPV_EVENT_END_FILE fires */
void keybinds_auto_next(AppState *s, Player *p);

#endif
