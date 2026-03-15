#include "player.h"

#include <mpv/client.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Player {
    mpv_handle *mpv;
    int         track_ended;
};

Player *player_create(void)
{
    Player *p = calloc(1, sizeof(Player));
    if (!p) return NULL;

    p->mpv = mpv_create();
    if (!p->mpv) {
        fprintf(stderr, "player_create: mpv_create() returned NULL\n");
        free(p);
        return NULL;
    }

    /* ---------------------------------------------------------------
     *      Only set options that the diagnostic proved safe.
     *      The diagnostic used terminal=yes/msg-level=all=info and worked.
     *      We flip those two to silence output — nothing else changes.
     *      --------------------------------------------------------------- */

    /* Silence all mpv output — must never write to the terminal
     *      while ncurses is running                                        */
    mpv_set_option_string(p->mpv, "terminal",  "no");
    mpv_set_option_string(p->mpv, "msg-level", "all=no");

    /* Audio-only: tell mpv not to touch any display/video subsystem  */
    mpv_set_option_string(p->mpv, "vid",       "no");

    /* Stay alive after a track ends so we can load the next one      */
    mpv_set_option_string(p->mpv, "idle",      "yes");

    /* Explicitly use pipewire — confirmed working on your system     */
    mpv_set_option_string(p->mpv, "ao",        "pipewire");

    int err = mpv_initialize(p->mpv);
    if (err < 0) {
        fprintf(stderr, "player_create: mpv_initialize() failed: %s\n",
                mpv_error_string(err));
        mpv_terminate_destroy(p->mpv);
        free(p);
        return NULL;
    }

    mpv_observe_property(p->mpv, 0, "time-pos",    MPV_FORMAT_DOUBLE);
    mpv_observe_property(p->mpv, 0, "duration",    MPV_FORMAT_DOUBLE);
    mpv_observe_property(p->mpv, 0, "pause",       MPV_FORMAT_FLAG);
    mpv_observe_property(p->mpv, 0, "idle-active", MPV_FORMAT_FLAG);

    return p;
}

void player_destroy(Player *p)
{
    if (!p) return;
    if (p->mpv) mpv_terminate_destroy(p->mpv);
    free(p);
}

void player_load(Player *p, const char *path)
{
    if (!p || !path) return;
    const char *cmd[] = { "loadfile", path, NULL };
    mpv_command(p->mpv, cmd);
    p->track_ended = 0;
}

void player_toggle_pause(Player *p)
{
    if (!p) return;
    int paused = player_is_paused(p);
    int new_val = paused ? 0 : 1;
    mpv_set_property(p->mpv, "pause", MPV_FORMAT_FLAG, &new_val);
}

void player_seek(Player *p, double seconds_relative)
{
    if (!p) return;
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", seconds_relative);
    const char *cmd[] = { "seek", buf, "relative", NULL };
    mpv_command(p->mpv, cmd);
}

void player_seek_abs(Player *p, double position)
{
    if (!p) return;
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", position);
    const char *cmd[] = { "seek", buf, "absolute", NULL };
    mpv_command(p->mpv, cmd);
}

void player_set_volume(Player *p, double vol)
{
    if (!p) return;
    if (vol < 0.0)   vol = 0.0;
    if (vol > 100.0) vol = 100.0;
    mpv_set_property(p->mpv, "volume", MPV_FORMAT_DOUBLE, &vol);
}

double player_get_position(Player *p)
{
    if (!p) return 0.0;
    double val = 0.0;
    mpv_get_property(p->mpv, "time-pos", MPV_FORMAT_DOUBLE, &val);
    return val;
}

double player_get_duration(Player *p)
{
    if (!p) return 0.0;
    double val = 0.0;
    mpv_get_property(p->mpv, "duration", MPV_FORMAT_DOUBLE, &val);
    return val;
}

double player_get_volume(Player *p)
{
    if (!p) return 100.0;
    double val = 100.0;
    mpv_get_property(p->mpv, "volume", MPV_FORMAT_DOUBLE, &val);
    return val;
}

int player_is_paused(Player *p)
{
    if (!p) return 0;
    int val = 0;
    mpv_get_property(p->mpv, "pause", MPV_FORMAT_FLAG, &val);
    return val;
}

int player_is_idle(Player *p)
{
    if (!p) return 1;
    int val = 1;
    mpv_get_property(p->mpv, "idle-active", MPV_FORMAT_FLAG, &val);
    return val;
}

int player_poll(Player *p)
{
    if (!p) return 0;

    p->track_ended = 0;

    while (1) {
        mpv_event *ev = mpv_wait_event(p->mpv, 0);

        if (ev->event_id == MPV_EVENT_NONE) {
            break;
        }
        if (ev->event_id == MPV_EVENT_END_FILE) {
            p->track_ended = 1;
        }
    }

    return p->track_ended;
}
