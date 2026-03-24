#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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
	/* 1. player before initscr() so errors print to terminal */
	Player *player = player_create();
	if (!player) return 1;

	/* 2. app state */
	AppState state;
	state_init(&state);

	/* 3. ncurses */
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

	/* 4. load tracks — NOW state is declared and ncurses is ready */
	if (argc >= 2) {
		/* arguments provided — files and/or directories */
		for (int i = 1; i < argc; i++) {
			struct stat st;
			if (stat(argv[i], &st) != 0) continue;

			if (S_ISDIR(st.st_mode)) {
				/* directory — scan recursively */
				scan_library(&state, argv[i]);
			} else {
				/* single file — add directly */
				if (state.track_count >= MAX_TRACKS) break;
				Track *t = &state.tracks[state.track_count];
				snprintf(t->path, sizeof(t->path), "%s", argv[i]);
				const char *base = strrchr(argv[i], '/');
				base = base ? base + 1 : argv[i];
				strncpy(t->title, base, sizeof(t->title) - 1);
				char *dot = strrchr(t->title, '.');
				if (dot) *dot = '\0';
				state.track_count++;
			}
		}
		snprintf(state.status_msg, sizeof(state.status_msg),
				 "%d track(s) loaded — press Enter to play",
				 state.track_count);
	} else {
		/* no arguments — read music_dir from config */
		char music_dir[1024];
		read_config(music_dir, sizeof(music_dir));

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

	/* 5. main loop */
	while (state.is_running) {

		int ended = player_poll(player);
		if (ended)
			playlist_next(&state, player);

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

		draw_all(&layout, &state);
		doupdate();

		int ch = getch();

		if (ch == KEY_RESIZE) {
			getmaxyx(stdscr, state.rows, state.cols);
			clear();
			layout_rebuild(&layout);
			continue;
		}

		handle_input(&state, player, ch);
	}

	/* 6. cleanup */
	layout_destroy(&layout);
	player_destroy(player);
	endwin();
	return 0;
}
