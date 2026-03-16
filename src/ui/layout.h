#ifndef LAYOUT_H
#define LAYOUT_H

#include <ncurses.h>

typedef struct {
    WINDOW *header;      /* row 0           — 1 row,  full width      */
    WINDOW *library;     /* rows 1..N-3     — left pane  (1/3 width)  */
    WINDOW *nowplaying;  /* rows 1..N-3     — right pane (2/3 width)  */
    WINDOW *status;      /* row N-2         — 1 row,  full width      */
    WINDOW *controls;    /* row N-1         — 1 row,  full width      */

    int rows;            /* terminal rows at last build                */
    int cols;            /* terminal cols at last build                */
    int sidebar_w;       /* library pane width                         */
    int content_w;       /* now-playing pane width                     */
    int content_h;       /* both panes height                          */
    int divider_col;     /* column of vertical divider                 */
} Layout;

void layout_create(Layout *l);
void layout_destroy(Layout *l);
void layout_rebuild(Layout *l);

#endif
