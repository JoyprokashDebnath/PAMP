#include "state.h"
#include <string.h>
#include <stdio.h>

void state_init(AppState *s)
{
    memset(s, 0, sizeof(AppState));
    s->is_running      = 1;
    s->volume          = 100.0;
    s->playback_state  = PLAYER_IDLE;
    s->repeat          = REPEAT_OFF;
    s->lib_selected    = 0;
    s->lib_scroll      = 0;
    s->current_index   = -1;
    snprintf(s->status_msg, sizeof(s->status_msg),
             "Welcome to PAMP");
}
