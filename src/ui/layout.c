#include "layout.h"

void layout_create(Layout *l)
{
    int rows = LINES;
    int cols = COLS;

    l->rows       = rows;
    l->cols       = cols;

    /* sidebar = 1/3 of screen, clamped between 22 and 48 cols */
    l->sidebar_w  = cols / 3;
    if (l->sidebar_w < 22) l->sidebar_w = 22;
    if (l->sidebar_w > 48) l->sidebar_w = 48;

    l->content_w   = cols - l->sidebar_w;
    l->content_h   = rows - 3;   /* header(1) + status(1) + controls(1) */
    l->divider_col = l->sidebar_w;

    l->header     = newwin(1,            cols,            0,          0);
    l->library    = newwin(l->content_h, l->sidebar_w,   1,          0);
    l->nowplaying = newwin(l->content_h, l->content_w,   1,          l->sidebar_w);
    l->status     = newwin(1,            cols,            rows - 2,   0);
    l->controls   = newwin(1,            cols,            rows - 1,   0);

    /* enable arrow keys on every window */
    keypad(l->header,     TRUE);
    keypad(l->library,    TRUE);
    keypad(l->nowplaying, TRUE);
    keypad(l->status,     TRUE);
    keypad(l->controls,   TRUE);
}

void layout_destroy(Layout *l)
{
    if (l->controls)   { delwin(l->controls);   l->controls   = NULL; }
    if (l->status)     { delwin(l->status);     l->status     = NULL; }
    if (l->nowplaying) { delwin(l->nowplaying); l->nowplaying = NULL; }
    if (l->library)    { delwin(l->library);    l->library    = NULL; }
    if (l->header)     { delwin(l->header);     l->header     = NULL; }
}

void layout_rebuild(Layout *l)
{
    layout_destroy(l);
    layout_create(l);
}
