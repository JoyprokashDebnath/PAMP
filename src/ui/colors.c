#include "colors.h"
#include <ncurses.h>

void init_colors(void)
{
    if (!has_colors()) return;
    start_color();
    use_default_colors();

    init_pair(PAIR_NORMAL,   -1,           -1          );
    init_pair(PAIR_SELECTED, COLOR_BLACK,  COLOR_WHITE );
    init_pair(PAIR_HEADER,   COLOR_WHITE,  COLOR_BLUE  );
    init_pair(PAIR_STATUS,   COLOR_BLACK,  COLOR_CYAN  );
    init_pair(PAIR_PROGRESS, COLOR_GREEN,  -1          );
    init_pair(PAIR_DIM,      COLOR_WHITE,  -1          );
    init_pair(PAIR_ERROR,    COLOR_RED,    -1          );
    init_pair(PAIR_HINTS,    COLOR_YELLOW, -1          );
    init_pair(PAIR_PLAYING,  COLOR_GREEN,  -1          );
    init_pair(PAIR_PAUSED,   COLOR_YELLOW, -1          );
    init_pair(PAIR_TITLE,    COLOR_CYAN,   -1          );
    init_pair(PAIR_BORDER,   COLOR_WHITE,  -1          );
}
