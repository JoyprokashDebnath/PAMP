#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio/player.h"
#include "audio/playlist.h"
#include "ui/layout.h"
#include "ui/draw.h"
#include "ui/colors.h"
#include "input/keybinds.h"
#include "state/state.h"
#include "library/scanner.h"

/* ------------------------------------------------------------------ */
/*  Config reader                                                       */
/* ------------------------------------------------------------------ */

static void read_config(char *music_dir, int size)
{
	const char *home = getenv("HOME");
	if (home)
		snprintf(music_dir, size, "%s/Music", home);
	else
		snprintf(music_dir, size, ".");

	char path[512];
	if (home)
		snprintf(path, sizeof(path), "%s/.config/pamp/config.ini", home);
	else
		return;

	FILE *f = fopen(path, "r");
	if (!f) f = fopen("config.ini", "r");
	if (!f) return;

	char line[512];
	while (fgets(line, sizeof(line), f)) {
		char *nl = strchr(line, '\n');
		if (nl) *nl = '\0';
		if (line[0] == ';' || line[0] == '#' || line[0] == '[') continue;

		char val[512];
		if (sscanf(line, " music_dir = %511[^\n;]", val) == 1) {
			/* trim trailing space */
			int len = (int)strlen(val);
			while (len > 0 && val[len - 1] == ' ') val[--len] = '\0';

			if (val[0] == '~' && home)
				snprintf(music_dir, size, "%s%s", home, val + 1);
			else
				strncpy(music_dir, val, size - 1);
		}
	}
	fclose(f);
}

/* ------------------------------------------------------------------ */
/*  Main                                                                */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
	/* player must be created before initscr() so errors are visible */
	Player *player = player_create();
	if (!player) return 1;

	AppState state;
	state_init(&state);

	/* determine music source */
	char music_dir[1024];
	int  single_file = (argc >= 2);

	if (single_file)
		snprintf(music_dir, sizeof(music_dir), "%s", argv[1]);
	else
		read_config(music_dir, sizeof(music_dir));

	/* ncurses setup */
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	timeout(50);

	init_colors();

	Layout layout;
	layout_create(&layout);
	getmaxyx(stdscr, state.rows, state.cols);

	/* load tracks */
	if (single_file) {
		Track *t = &state.tracks[0];
		snprintf(t->path, sizeof(t->path), "%s", argv[1]);
		/* strip extension for display title */
		const char *base = strrchr(argv[1], '/');
		base = base ? base + 1 : argv[1];
		strncpy(t->title, base, sizeof(t->title) - 1);
		char *dot = strrchr(t->title, '.');
		if (dot) *dot = '\0';
		state.track_count = 1;
		snprintf(state.status_msg, sizeof(state.status_msg),
				 "1 track — press Enter or Space to play");
	} else {
		/* show scanning message before blocking scan */
		snprintf(state.status_msg, sizeof(state.status_msg),
				 "Scanning library...");
		draw_all(&layout, &state);
		doupdate();

		int found = scan_library(&state, music_dir);

		if (found > 0)
			snprintf(state.status_msg, sizeof(state.status_msg),
					 "%d tracks loaded", found);
		else
				snprintf(state.status_msg, sizeof(state.status_msg),
						 "No tracks found — check music_dir in config.ini");
	}

	/* main loop */
	while (state.is_running) {

		/* drain mpv events */
		int ended = player_poll(player);
		if (ended)
			playlist_next(&state, player);

		/* sync live position/volume from mpv */
		if (state.playback_state == PLAYER_PLAYING ||
			state.playback_state == PLAYER_PAUSED)
		{
			state.position = player_get_position(player);
			state.duration = player_get_duration(player);
			state.volume   = player_get_volume(player);

			if (player_is_paused(player))
				state.playback_state = PLAYER_PAUSED;
			else if (state.playback_state == PLAYER_PAUSED)
				state.playback_state = PLAYER_PLAYING;
		}

		/* render */
		draw_all(&layout, &state);
		doupdate();

		/* input */
		int ch = getch();

		if (ch == KEY_RESIZE) {
			getmaxyx(stdscr, state.rows, state.cols);
			clear();
			layout_rebuild(&layout);
			continue;
		}

		handle_input(&state, player, ch);
	}

	layout_destroy(&layout);
	player_destroy(player);
	endwin();
	return 0;
}
