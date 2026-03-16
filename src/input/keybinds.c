#include "keybinds.h"
#include "../audio/playlist.h"
#include <ncurses.h>
#include <string.h>
#include <stdio.h>

void handle_input(AppState *s, Player *p, int ch)
{
    switch (ch) {

        case 'q':
        case 'Q':
            s->is_running = 0;
            break;

        case ' ':
            if (s->playback_state == PLAYER_PLAYING) {
                player_toggle_pause(p);
                s->playback_state = PLAYER_PAUSED;
                snprintf(s->status_msg, sizeof(s->status_msg), "Paused");
            } else if (s->playback_state == PLAYER_PAUSED) {
                player_toggle_pause(p);
                s->playback_state = PLAYER_PLAYING;
                snprintf(s->status_msg, sizeof(s->status_msg),
                         "Playing: %.*s",
                         (int)sizeof(s->status_msg) - 10,
                         s->current_title);
            } else if (s->playback_state == PLAYER_IDLE &&
                s->track_count > 0) {
                playlist_play_index(s, p, s->lib_selected);
                }
                break;

        case '\n':
        case KEY_ENTER:
            if (s->lib_selected < s->track_count)
                playlist_play_index(s, p, s->lib_selected);
        break;

        case 'n':
            playlist_next(s, p);
            break;

        case 'N':
            playlist_prev(s, p);
            break;

        case KEY_RIGHT:
            player_seek(p, +5.0);
            snprintf(s->status_msg, sizeof(s->status_msg), "Seek +5s");
            break;

        case KEY_LEFT:
            player_seek(p, -5.0);
            snprintf(s->status_msg, sizeof(s->status_msg), "Seek -5s");
            break;

        case ']':
            s->volume += 5.0;
            if (s->volume > 100.0) s->volume = 100.0;
            player_set_volume(p, s->volume);
        snprintf(s->status_msg, sizeof(s->status_msg),
                 "Volume: %.0f%%", s->volume);
        break;

        case '[':
            s->volume -= 5.0;
            if (s->volume < 0.0) s->volume = 0.0;
            player_set_volume(p, s->volume);
        snprintf(s->status_msg, sizeof(s->status_msg),
                 "Volume: %.0f%%", s->volume);
        break;

        case 'm':
            s->muted = !s->muted;
            player_set_volume(p, s->muted ? 0.0 : s->volume);
            snprintf(s->status_msg, sizeof(s->status_msg),
                     s->muted ? "Muted" : "Unmuted");
            break;

        case KEY_UP:
        case 'k':
            if (s->lib_selected > 0) s->lib_selected--;
            break;

        case KEY_DOWN:
        case 'j':
            if (s->lib_selected < s->track_count - 1)
                s->lib_selected++;
        break;

        case 'g':
            s->lib_selected = 0;
            s->lib_scroll   = 0;
            break;

        case 'G':
            s->lib_selected = s->track_count > 0
            ? s->track_count - 1 : 0;
            break;

        case 's':
            s->shuffle = !s->shuffle;
            snprintf(s->status_msg, sizeof(s->status_msg),
                     "Shuffle: %s", s->shuffle ? "on" : "off");
            break;

        case 'r':
            s->repeat = (s->repeat + 1) % 3;
            snprintf(s->status_msg, sizeof(s->status_msg),
                     "Repeat: %s",
                     s->repeat == REPEAT_OFF ? "off"
                     : s->repeat == REPEAT_ALL ? "all" : "one");
            break;

        default:
            break;
    }
}
