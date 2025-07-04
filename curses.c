#include <curl/curl.h>
#include "main.h"

int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;
int WINDOW_POSX = 0;
int WINDOW_POSY = 0;

int WINDOW_SIZEX = 50;
int WINDOW_SIZEY = 25;

void sleep_ms(int ms){ usleep(ms * 1000); }
void mainloop(){
	BlockText items[256] = {};
	int itemlen = 0;
	int should_exit = false;
	int offset = 5;

	// make a window
	WINDOW *main_window;
	WINDOW *border_window;
	WINDOW_POSX = (SCREEN_WIDTH - WINDOW_SIZEX)/2;
	WINDOW_POSY = (SCREEN_HEIGHT - WINDOW_SIZEY)/2;

	main_window = newwin(25, 58, WINDOW_POSY, WINDOW_POSX);
	border_window = newwin(25+4, 58+4, WINDOW_POSY-2, WINDOW_POSX-2);

	keypad(main_window, true);
	//nodelay(main_window, true);
	refresh();

	while(!should_exit){
		int c = wgetch(main_window);
		wclear(main_window);

		switch (c) {
				case 'q':
					should_exit = true;
				break;

				case KEY_DOWN:
					offset-=4;
					break;

				case KEY_UP:
					offset+=4;
					break;

				default:
					mvprintw(WINDOW_POSY, WINDOW_POSX+2, "PRESSED: %d", c);
					break;
		}

		box(border_window,0,0);
		wrefresh(border_window);

		int pos = 0;
		int highlight = 0;
		char buff[150] = {0};

		for(int i = 0; i < itemlen; i++){
			pos = (i*4) + offset;
			if (pos > 45) continue;

			int linelen = firstline(items[i].content);
			if (linelen > 140) linelen = 140;
			memcpy(buff, items[i].content, linelen);
			buff[140] = 0;

			if (pos > 8 && pos < 13) { wattron(main_window, COLOR_PAIR(2)); }
			mvwprintw(main_window, pos, 0, "%s", buff);
			if (pos > 8 && pos < 13) { wattroff(main_window, COLOR_PAIR(2)); }

			highlight = 0;
		}

		wrefresh(main_window);
		//refresh();
	}
}

int firstline(char* str){
	int count = 0;
	char c = str[count];
	while (c!='\0' && c!='\n'){
		c = str[++count];
	}

	return count;
}

void endgame(){clear(); endwin();}

void init(){
	initscr();
	curs_set(0);
	noecho();
	cbreak();
	start_color();			/* Start color 			*/

	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_GREEN, COLOR_RED);

	clear();

	// Get screen width and height
	getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
}
