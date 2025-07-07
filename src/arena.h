#ifndef ARENA
#define ARENA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "cJSON.h"

CURL* GETCURL(char *url, struct curl_slist* headers, struct memory *chunk);
struct curl_slist* json_headers();

cJSON* get_channel(char* slug);
cJSON* get_block(char* slugorid);
CURL* get_channel_curl(char* slugorid);

// allocates for you
struct memory_list MULTIGET(char **url_list);

typedef struct {
	char *title;
	char *content;
} BlockText;

int firstline(char* str);

#endif
