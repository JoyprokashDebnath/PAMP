#include "draw.h"
#include "colors.h"
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Primitives                                                          */
/* ------------------------------------------------------------------ */

void fill_line(WINDOW *win, int row, chtype ch, int pair)
{
    int w = getmaxx(win);
    wattron(win, COLOR_PAIR(pair));
    wmove(win, row, 0);
    for (int i = 0; i < w; i++) waddch(win, ch);
    wattroff(win, COLOR_PAIR(pair));
}

void print_truncated(WINDOW *win, int row, int col, int max_w, const char *str)
{
    if (max_w <= 0 || !str) return;
    if ((int)strlen(str) <= max_w)
        mvwprintw(win, row, col, "%s", str);
    else
        mvwprintw(win, row, col, "%.*s...", max_w - 3, str);
}

void draw_progress_bar(WINDOW *win, int row, int col,
                       int width, double pos, double dur)
{
    if (width < 10) return;

    int bar_x = col + 6;
    int bar_w = width - 12;
    if (bar_w < 2) bar_w = 2;

    /* use a char array written manually — avoids all snprintf range warnings */
    char ts[8] = "--:--  ";
    char te[8] = "--:--  ";

    if (dur > 0 && pos >= 0) {
        unsigned int ps = (unsigned int)pos;
        unsigned int ds = (unsigned int)dur;
        unsigned int pm = (ps / 60u) % 100u;   /* cap at 99 min */
        unsigned int pp = ps % 60u;
        unsigned int dm = (ds / 60u) % 100u;
        unsigned int dp = ds % 60u;
        ts[0] = '0' + pm / 10; ts[1] = '0' + pm % 10;
        ts[2] = ':';
        ts[3] = '0' + pp / 10; ts[4] = '0' + pp % 10;
        ts[5] = '\0';
        te[0] = '0' + dm / 10; te[1] = '0' + dm % 10;
        te[2] = ':';
        te[3] = '0' + dp / 10; te[4] = '0' + dp % 10;
        te[5] = '\0';
    }

    mvwprintw(win, row, col, "%s", ts);
    mvwprintw(win, row, bar_x + bar_w + 1, "%s", te);

    double ratio = (dur > 0) ? pos / dur : 0.0;
    int filled   = (int)(ratio * bar_w);
    if (filled > bar_w) filled = bar_w;
    if (filled < 0)     filled = 0;

    wattron(win, COLOR_PAIR(PAIR_PROGRESS) | A_BOLD);
    wmove(win, row, bar_x);
    for (int i = 0; i < bar_w; i++) {
        if      (i < filled)   waddstr(win, "█");
        else if (i == filled)  waddstr(win, "▓");
        else                   waddstr(win, "░");
    }
    wattroff(win, COLOR_PAIR(PAIR_PROGRESS) | A_BOLD);
}

void draw_volume_bar(WINDOW *win, int row, int col, int width, double vol)
{
    if (width < 6) return;
    int bar_w  = width - 9;
    if (bar_w < 2) bar_w = 2;
    int filled = (int)(vol / 100.0 * bar_w);
    if (filled > bar_w) filled = bar_w;
    if (filled < 0)     filled = 0;

    mvwprintw(win, row, col, "vol ");
    wattron(win, COLOR_PAIR(PAIR_PROGRESS));
    wmove(win, row, col + 4);
    for (int i = 0; i < bar_w; i++)
        waddstr(win, i < filled ? "█" : "░");
    wattroff(win, COLOR_PAIR(PAIR_PROGRESS));
    mvwprintw(win, row, col + 4 + bar_w + 1, "%3.0f%%", vol);
}

/* ------------------------------------------------------------------ */
/*  Header                                                              */
/* ------------------------------------------------------------------ */

void draw_header(WINDOW *win, Layout *l, AppState *s)
{
    int w = l->cols;
    werase(win);
    fill_line(win, 0, ' ', PAIR_HEADER);

    wattron(win, COLOR_PAIR(PAIR_HEADER) | A_BOLD);
    mvwprintw(win, 0, 2, "PAMP");
    wattroff(win, A_BOLD);

    if (s->playback_state == PLAYER_PLAYING ||
        s->playback_state == PLAYER_PAUSED)
    {
        const char *title = s->current_title[0]
        ? s->current_title : s->current_path;
        int max_t = w - 30;
        if (max_t > 4)
            print_truncated(win, 0, (w - (int)strlen(title)) / 2,
                            max_t, title);
    }

    int rx = w - 16;
    if (rx > 8) {
        mvwprintw(win, 0, rx,     "%s", s->shuffle ? "[shuf]" : "      ");
        mvwprintw(win, 0, rx + 7, "%s",
                  s->repeat == REPEAT_ONE ? "[1]"
                  : s->repeat == REPEAT_ALL ? "[A]" : "   ");
    }
    wattroff(win, COLOR_PAIR(PAIR_HEADER));
}

/* ------------------------------------------------------------------ */
/*  Library pane                                                        */
/* ------------------------------------------------------------------ */

void draw_library(WINDOW *win, Layout *l, AppState *s)
{
    int h = l->content_h;
    int w = l->sidebar_w;
    werase(win);

    wattron(win, COLOR_PAIR(PAIR_BORDER));
    for (int r = 0; r < h; r++)
        mvwaddch(win, r, w - 1, ACS_VLINE);
    wattroff(win, COLOR_PAIR(PAIR_BORDER));

    wattron(win, A_BOLD | COLOR_PAIR(PAIR_TITLE));
    mvwprintw(win, 0, 1, "Library");
    wattroff(win, A_BOLD | COLOR_PAIR(PAIR_TITLE));

    wattron(win, COLOR_PAIR(PAIR_DIM));
    mvwprintw(win, 1, 1, "%-*s", w - 3, "───────────────────");
    wattroff(win, COLOR_PAIR(PAIR_DIM));

    int visible = h - 3;
    int max_w   = w - 4;

    if (s->track_count == 0) {
        wattron(win, COLOR_PAIR(PAIR_DIM) | A_DIM);
        mvwprintw(win, 3, 1, "No tracks loaded");
        mvwprintw(win, 5, 1, "Set music_dir in");
        mvwprintw(win, 6, 1, "config.ini");
        wattroff(win, COLOR_PAIR(PAIR_DIM) | A_DIM);
        return;
    }

    if (s->lib_selected < s->lib_scroll)
        s->lib_scroll = s->lib_selected;
    if (s->lib_selected >= s->lib_scroll + visible)
        s->lib_scroll = s->lib_selected - visible + 1;
    if (s->lib_scroll < 0) s->lib_scroll = 0;

    for (int i = 0; i < visible; i++) {
        int idx = i + s->lib_scroll;
        if (idx >= s->track_count) break;

        int row         = i + 2;
        int is_selected = (idx == s->lib_selected);
        int is_playing  = (idx == s->current_index &&
        s->playback_state != PLAYER_IDLE);

        const char *name = s->tracks[idx].title[0]
        ? s->tracks[idx].title
        : s->tracks[idx].path;
        const char *slash = strrchr(name, '/');
        if (slash) name = slash + 1;

        if (is_selected) {
            wattron(win, COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
            mvwprintw(win, row, 1, " %-*.*s", max_w, max_w, name);
            wattroff(win, COLOR_PAIR(PAIR_SELECTED) | A_BOLD);
        } else if (is_playing) {
            wattron(win, COLOR_PAIR(PAIR_PLAYING) | A_BOLD);
            mvwprintw(win, row, 1, ">%-*.*s", max_w, max_w, name);
            wattroff(win, COLOR_PAIR(PAIR_PLAYING) | A_BOLD);
        } else {
            wattron(win, COLOR_PAIR(PAIR_NORMAL));
            mvwprintw(win, row, 1, " %-*.*s", max_w, max_w, name);
            wattroff(win, COLOR_PAIR(PAIR_NORMAL));
        }
    }

    wattron(win, COLOR_PAIR(PAIR_DIM) | A_DIM);
    mvwprintw(win, h - 1, 1, "%d/%d",
              s->lib_selected + 1, s->track_count);
    wattroff(win, COLOR_PAIR(PAIR_DIM) | A_DIM);
}

/* ------------------------------------------------------------------ */
/*  Now-playing pane                                                    */
/* ------------------------------------------------------------------ */

void draw_nowplaying(WINDOW *win, Layout *l, AppState *s)
{
    int h = l->content_h;
    int w = l->content_w;
    werase(win);

    wattron(win, A_BOLD | COLOR_PAIR(PAIR_TITLE));
    mvwprintw(win, 0, 2, "Now Playing");
    wattroff(win, A_BOLD | COLOR_PAIR(PAIR_TITLE));
    wattron(win, COLOR_PAIR(PAIR_DIM));
    mvwprintw(win, 1, 2, "───────────────────────────────");
    wattroff(win, COLOR_PAIR(PAIR_DIM));

    if (s->playback_state == PLAYER_IDLE) {
        int mid = h / 2;
        wattron(win, COLOR_PAIR(PAIR_DIM) | A_DIM);
        mvwprintw(win, mid,     (w - 15) / 2, "Nothing playing");
        mvwprintw(win, mid + 2, (w - 22) / 2, "Press Enter on a track");
        wattroff(win, COLOR_PAIR(PAIR_DIM) | A_DIM);
        return;
    }

    int mid = h / 2 - 2;
    if (mid < 3) mid = 3;

    /* title */
    const char *title = s->current_title[0]
    ? s->current_title : "Unknown Title";
    wattron(win, COLOR_PAIR(PAIR_TITLE) | A_BOLD);
    print_truncated(win, mid, 2, w - 4, title);
    wattroff(win, COLOR_PAIR(PAIR_TITLE) | A_BOLD);

    /* artist - album */
    char meta[256];
    meta[0] = '\0';
    if (s->current_artist[0] && s->current_album[0]) {
        snprintf(meta, sizeof(meta), "%.*s - %.*s",
                 120, s->current_artist, 120, s->current_album);
    } else if (s->current_artist[0]) {
        snprintf(meta, sizeof(meta), "%.*s", 240, s->current_artist);
    } else if (s->current_album[0]) {
        snprintf(meta, sizeof(meta), "%.*s", 240, s->current_album);
    }
    if (meta[0]) {
        wattron(win, COLOR_PAIR(PAIR_DIM));
        print_truncated(win, mid + 1, 2, w - 4, meta);
        wattroff(win, COLOR_PAIR(PAIR_DIM));
    }

    /* state badge */
    if (s->playback_state == PLAYER_PAUSED) {
        wattron(win, COLOR_PAIR(PAIR_PAUSED) | A_REVERSE | A_BOLD);
        mvwprintw(win, mid + 3, 2, "  PAUSED  ");
        wattroff(win, COLOR_PAIR(PAIR_PAUSED) | A_REVERSE | A_BOLD);
    } else {
        wattron(win, COLOR_PAIR(PAIR_PLAYING) | A_REVERSE | A_BOLD);
        mvwprintw(win, mid + 3, 2, " PLAYING  ");
        wattroff(win, COLOR_PAIR(PAIR_PLAYING) | A_REVERSE | A_BOLD);
    }

    /* progress bar */
    int bar_row = mid + 5;
    if (bar_row < h - 4) {
        draw_progress_bar(win, bar_row, 2, w - 6, s->position, s->duration);
    }

    /* volume bar */
    int vol_row = bar_row + 2;
    int vol_w   = w / 2;
    if (vol_row < h - 2 && vol_w > 10) {
        draw_volume_bar(win, vol_row, 2, vol_w, s->volume);
    }
}

/* ------------------------------------------------------------------ */
/*  Status bar                                                          */
/* ------------------------------------------------------------------ */

void draw_status(WINDOW *win, Layout *l, AppState *s)
{
    int w = l->cols;
    werase(win);
    fill_line(win, 0, ' ', PAIR_STATUS);
    wattron(win, COLOR_PAIR(PAIR_STATUS));
    mvwprintw(win, 0, 2, "%-*.*s", w - 4, w - 4, s->status_msg);
    wattroff(win, COLOR_PAIR(PAIR_STATUS));
}

/* ------------------------------------------------------------------ */
/*  Controls bar                                                        */
/* ------------------------------------------------------------------ */

void draw_controls(WINDOW *win, Layout *l)
{
    int w = l->cols;
    werase(win);
    wattron(win, COLOR_PAIR(PAIR_HINTS) | A_DIM);
    mvwprintw(win, 0, 1, "%-*.*s", w - 2, w - 2,
              "[Enter] play  [Space] pause  [</>] seek  "
              "[up/dn] nav  [n/N] next/prev  "
              "[s] shuffle  [r] repeat  [[/]] vol  [q] quit");
    wattroff(win, COLOR_PAIR(PAIR_HINTS) | A_DIM);
}

/* ------------------------------------------------------------------ */
/*  draw_all                                                            */
/* ------------------------------------------------------------------ */

void draw_all(Layout *l, AppState *s)
{
    draw_header(l->header,         l, s);
    draw_library(l->library,       l, s);
    draw_nowplaying(l->nowplaying, l, s);
    draw_status(l->status,         l, s);
    draw_controls(l->controls,     l);

    wnoutrefresh(l->header);
    wnoutrefresh(l->library);
    wnoutrefresh(l->nowplaying);
    wnoutrefresh(l->status);
    wnoutrefresh(l->controls);
}
