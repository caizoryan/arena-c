#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>
#include <curl/curl.h>
#include "cJSON.h"
#define true 1
#define false 0

typedef struct position {
	int x;
	int y;
} position;

struct memory {
  char *response;
  size_t size;
};


struct memory_list {
  struct memory* list;
  size_t size;
};

void init();
void sleep_ms(int ms);

// Procedures
void createfood();
void mainloop();
void endgame();
void filestuff();

CURL* GETCURL(char *url, struct curl_slist* headers, struct memory *chunk);
struct curl_slist* json_headers();

cJSON* get_channel(char* slug);
cJSON* get_block(char* slugorid);
CURL* get_channel_curl(char* slugorid);

// allocates for you
struct memory_list MULTIGET(char **url_list);


typedef struct {
	char *title;
	int id;
} Block;

typedef struct {
} BlockAttachment;

typedef struct {
} BlockLink;

typedef struct {
} BlockImage;

typedef struct {
	char *title;
	char *content;
} BlockText;

int firstline(char* str);


#endif
