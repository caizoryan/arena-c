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

int measure(char *string);
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

// allocates for you
struct memory_list MULTIGET(char **url_list);

#endif
