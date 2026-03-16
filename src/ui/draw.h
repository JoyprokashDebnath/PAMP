#ifndef DRAW_H
#define DRAW_H

#include <ncurses.h>
#include "layout.h"
#include "../state/state.h"

/* Render every window — call once per frame, then doupdate() */
void draw_all(Layout *l, AppState *s);

/* Individual window renders */
void draw_header(WINDOW *win, Layout *l, AppState *s);
void draw_library(WINDOW *win, Layout *l, AppState *s);
void draw_nowplaying(WINDOW *win, Layout *l, AppState *s);
void draw_status(WINDOW *win, Layout *l, AppState *s);
void draw_controls(WINDOW *win, Layout *l);

/* Primitives used by draw.c and optionally by popup dialogs */
void draw_progress_bar(WINDOW *win, int row, int col,
                       int width, double pos, double dur);
void draw_volume_bar(WINDOW *win, int row, int col,
                     int width, double vol);
void fill_line(WINDOW *win, int row, chtype ch, int pair);
void print_truncated(WINDOW *win, int row, int col,
                     int max_w, const char *str);

#endif
