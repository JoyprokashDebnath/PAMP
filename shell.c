#include <ncurses.h>
#include <locale.h>
#include <string.h>

int main(){

	int ch;
	int row, column;

    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();

	start_color();
	use_default_colors();
	init_pair(1, COLOR_RED , -1);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLUE);

	int running = 1;

	getmaxyx(stdscr, row, column);

	const char* logo = "PAMP";
	int logo_length = strlen(logo);

	attron( A_UNDERLINE | A_BOLD | COLOR_PAIR(1));

	mvprintw(0,(column - logo_length)/2,"%s",logo);

	attroff(A_UNDERLINE| A_BOLD |COLOR_PAIR(1));

	int win_h = 10, win_w = 40;
	int win_y = (row - win_h)/2;
	int win_x = (column - win_w)/2;

	WINDOW *popup = newwin(win_h,win_w ,win_y ,win_x );

	ch = getch();

	keypad(stdscr, TRUE);

	switch (ch) {
		case KEY_UP: break;
		case KEY_DOWN: break;
		case KEY_LEFT: break;
		case KEY_RIGHT: break;

	}
    curs_set(0);
    timeout(50);

	while (running){
		int inp = getch();

		if (inp == 'q') {
			endwin();
	}

		else{
			mvprintw(1,column / 2, "%c", inp);

	}
	}

    if (has_colors()) {


	}
	refresh();

	switch (ch) {
		case 'q': endwin();
	}



	return 0;
}
