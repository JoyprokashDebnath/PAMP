#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "audio/player.h"

static void draw_progress(WINDOW *win, int row, int col, int width,
						  double pos, double dur)
{
	if (dur <= 0.0) {
		mvwprintw(win, row, col, "%-*s", width, " --:-- / --:--");
		return;
	}

	double ratio  = pos / dur;
	int    filled = (int)(ratio * width);
	if (filled > width) filled = width;

	int pm = (int)pos / 60, ps = (int)pos % 60;
	int dm = (int)dur / 60, ds = (int)dur % 60;

	wmove(win, row, col);
	for (int i = 0; i < width; i++) {
		if      (i < filled)  waddstr(win, "█");
		else if (i == filled) waddstr(win, "▓");
		else                  waddstr(win, "░");
	}

	mvwprintw(win, row + 1, col,
			  "%02d:%02d / %02d:%02d", pm, ps, dm, ds);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: pamp <audio-file>\n");
		return 1;
	}

	const char *filepath = argv[1];

	/* Init player BEFORE ncurses so error output is visible in terminal */
	Player *player = player_create();
	if (!player) {
		return 1;
	}

	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	timeout(50);

	if (has_colors()) {
		start_color();
		use_default_colors();
		init_pair(1, COLOR_GREEN,  -1);
		init_pair(2, COLOR_CYAN,   -1);
		init_pair(3, COLOR_YELLOW, -1);
		init_pair(4, COLOR_WHITE,  -1);
	}

	player_load(player, filepath);

	int running = 1;
	while (running) {

		int ended = player_poll(player);
		if (ended) {
			running = 0;
		}

		double pos    = player_get_position(player);
		double dur    = player_get_duration(player);
		double vol    = player_get_volume(player);
		int    paused = player_is_paused(player);

		erase();
		int rows, cols;
		getmaxyx(stdscr, rows, cols);
		(void)rows;

		attron(COLOR_PAIR(2) | A_BOLD);
		mvprintw(1, 2, "PAMP — Phase 1 audio test");
		attroff(COLOR_PAIR(2) | A_BOLD);

		attron(COLOR_PAIR(4));
		mvprintw(2, 2, "File: %s", filepath);
		attroff(COLOR_PAIR(4));

		attron(COLOR_PAIR(3));
		mvprintw(4, 2, "Status: %s", paused ? "PAUSED " : "PLAYING");
		mvprintw(4, 22, "Volume: %.0f%%", vol);
		attroff(COLOR_PAIR(3));

		int bar_w = cols - 4;
		if (bar_w < 4) bar_w = 4;
		attron(COLOR_PAIR(1));
		draw_progress(stdscr, 6, 2, bar_w, pos, dur);
		attroff(COLOR_PAIR(1));

		attron(A_DIM);
		mvprintw(9, 2, "[space] pause   [</>] seek   [[]]] volume   [q] quit");
		attroff(A_DIM);

		refresh();

		int ch = getch();
		switch (ch) {
			case ' ':       player_toggle_pause(player);        break;
			case KEY_RIGHT: player_seek(player, +5.0);          break;
			case KEY_LEFT:  player_seek(player, -5.0);          break;
			case ']':       player_set_volume(player, vol+5.0); break;
			case '[':       player_set_volume(player, vol-5.0); break;
			case 'q':
			case 'Q':       running = 0;                        break;
			default:        break;
		}
	}

	player_destroy(player);
	endwin();
	return 0;
}
