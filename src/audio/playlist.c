#include "playlist.h"
#include <string.h>
#include <stdio.h>

void playlist_play_index(AppState *s, Player *p, int index)
{
    if (index < 0 || index >= s->track_count) return;
    s->current_index  = index;
    s->lib_selected   = index;
    s->playback_state = PLAYER_PLAYING;

    player_load(p, s->tracks[index].path);

    strncpy(s->current_path,   s->tracks[index].path,
            sizeof(s->current_path)   - 1);
    strncpy(s->current_title,  s->tracks[index].title,
            sizeof(s->current_title)  - 1);
    strncpy(s->current_artist, s->tracks[index].artist,
            sizeof(s->current_artist) - 1);
    strncpy(s->current_album,  s->tracks[index].album,
            sizeof(s->current_album)  - 1);

    /* safe truncated status message */
    char name[128];
    strncpy(name,
            s->tracks[index].title[0]
            ? s->tracks[index].title
            : s->tracks[index].path,
            sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    snprintf(s->status_msg, sizeof(s->status_msg),
             "Now playing: %.*s", (int)sizeof(s->status_msg) - 15, name);
}

void playlist_next(AppState *s, Player *p)
{
    if (s->track_count == 0) return;
    if (s->repeat == REPEAT_ONE) {
        playlist_play_index(s, p, s->current_index);
        return;
    }
    int next = s->current_index + 1;
    if (next >= s->track_count) {
        if (s->repeat == REPEAT_ALL) next = 0;
        else {
            s->playback_state = PLAYER_IDLE;
            snprintf(s->status_msg, sizeof(s->status_msg),
                     "End of playlist");
            return;
        }
    }
    playlist_play_index(s, p, next);
}

void playlist_prev(AppState *s, Player *p)
{
    if (s->track_count == 0) return;
    int prev = s->current_index - 1;
    if (prev < 0) prev = s->track_count - 1;
    playlist_play_index(s, p, prev);
}
