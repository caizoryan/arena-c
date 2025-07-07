#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "main.h"
#include "arena.h"

struct curl_slist* json_headers(){
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    return headers;
}

CURL* GETCURL(char *url, struct curl_slist* headers, struct memory *chunk){
	CURL *curl;
	curl = curl_easy_init();

	if(curl) {
		// URL & Headers
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

		// handle writing data
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
	}
	return curl;
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
