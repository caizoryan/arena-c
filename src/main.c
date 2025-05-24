#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "channel.h"
#include "main.h"

int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;
int WINDOW_POSX = 0;
int WINDOW_POSY = 0;

int WINDOW_SIZEX = 50;
int WINDOW_SIZEY = 25;

position food;
void sleep_ms(int ms){ usleep(ms * 1000); }

struct curl_slist* json_headers(){
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    return headers;
}

void GET(char *url, struct curl_slist* headers, struct memory *chunk){
  CURL *curl;
  CURLcode res;
  curl = GETCURL(url, headers, chunk);

  if(curl) {
    /* Perform the request, res gets the return code */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if(res != CURLE_OK)
	    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
	}
}

CURL* GETCURL(char *url, struct curl_slist* headers, struct memory *chunk){
    CURL *curl;
    curl = curl_easy_init();

    if(curl) {
	// URL & Headers
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 


	// handle writing data
	/* curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb); */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
    }
    return curl;
}

CURL* get_channel_curl(char* slugorid){
    struct memory chunk = {0};

    char url[512] = "https://api.are.na/v2/channels/";
    strcat(url, slugorid);
    strcat(url, "?per=100");

    CURL* curl = GETCURL(url, json_headers(), &chunk);

    return curl;
}

cJSON* get_channel(char* slugorid){
    struct memory chunk = {0};
    cJSON* channel;

    char url[512] = "https://api.are.na/v2/channels/";
    strcat(url, slugorid);
    strcat(url, "?per=100");

    GET(url, json_headers(), &chunk);
    channel = cJSON_Parse(chunk.response);

    return channel;
}

cJSON* get_block(char* slugorid){
    struct memory chunk = {0};
    char url[512] = "https://api.are.na/v2/blocks/";
    strcat(url, slugorid);

    cJSON* block;

    GET(url, json_headers(), &chunk);
    block = cJSON_Parse(chunk.response);

    return block;
}

BlockText items[256] = {};
int itemlen = 0;

char *slugs[] = {
    "jose-aisle", 
    "list-websites-lwvz0acnwli",
    "list-are-na-api-possibilities",
    "list-are-na-gists",
};

struct ChannelRequestArr {
    ChannelRequest *buff[64];
    int len;
} requests; 

int main(int argc, char **argv) {
	/* init a multi stack */
	CURLM *multi_handle = curl_multi_init();
	for(size_t i = 0; i < sizeof(slugs)/sizeof(slugs[0]); i++){
	printf("line %ld\n", i);
	requests.buff[i] = construct_channel_request(slugs[i]);
	curl_multi_add_handle(multi_handle, requests.buff[i]->curl);
	++requests.len;
	}

	int still_running = 1;
	while(still_running) {
	CURLMsg *msg;
	int queued;
	CURLMcode mc = curl_multi_perform(multi_handle, &still_running);

	if(still_running)
			/* wait for activity, timeout or "nothing" */
			mc = curl_multi_poll(multi_handle, NULL, 0, 1000, NULL);

	if(mc) break;

	do {
			msg = curl_multi_info_read(multi_handle, &queued);
			if(msg && msg->msg == CURLMSG_DONE) {
			printf("Transfer completed\n");
			}
	} while(msg);
	}

	ChannelParsed channels[64];
	int channel_len = 0;
	for(int i = 0; i < requests.len; i++){
		ChannelParsed channel = parse_channel(requests.buff[i]); 
		channels[i] = channel;
		++channel_len;
    }

    /* init(); */
    /* mainloop(); */
    /* endgame(); */

    for(int i = 0; i < requests.len; i++){
		char *len = cJSON_Print(cJSON_GetObjectItem(channels[i].data, "length"));
		printf("%d. slug: %s\t", i, channels[i].slug);
		printf("length, %s\n", len);
		free(len);

		curl_multi_remove_handle(multi_handle, requests.buff[i]->curl);
		clean_channel_request(requests.buff[i]);
		cJSON_Delete(channels[i].data);
		free(channels[i].slug);
    }

    curl_multi_cleanup(multi_handle);

    return 0;
}

void mainloop(){
    int should_exit = false;
    int c = 0;
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

void endgame(){
	// clear and return
	clear();
	endwin();
}

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
