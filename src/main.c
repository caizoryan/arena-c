#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "channel.h"
#include "sqlite.h"
#include "main.h"


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

struct ChannelRequestArr {
		ChannelRequest *buff[128];
		int len;
} requests;

void process_multiple_channels(char *slugs[], int len,sqlite3 *db){
	/* init a multi stack */
	CURLM *multi_handle = curl_multi_init();
	for(int i = 0; i < len; i++){
		requests.buff[i] = construct_channel_request(slugs[i]);
		curl_multi_add_handle(multi_handle, requests.buff[i]->curl);
		++requests.len;
	}

	/* Loop to process requests */
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

	Channel channels[64];
	int channel_len = 0;

	/* Parse add and print channel */
	for(int i = 0; i < requests.len; i++){
		Channel channel = parse_channel(requests.buff[i]); 
		channels[i] = channel;
		++channel_len;

		addChannel(db, channel);
	}

	/* Clean up */
	for(int i = 0; i < requests.len; i++){
		Channel c = channels[i];

		printf("%d. CLEANING UP %s\t\n", i, c.slug);

		cleanChannel(c);

		/* printf("First Block: %s\nlen: %d\n", c.contents[0].title,c.contents_len); */

		/* free(c.slug); */
		/* free(c.title); */
		/* free(c.updated_at); */
		/* free(c.created_at); */
		/* free(c.status); */

		/* clean_channel_request(requests.buff[i]); */
		/* curl_multi_remove_handle(multi_handle, requests.buff[i]->curl); */

		/* for(int i = 0; i < c.contents_len; i++){ */
		/* 	Block block = c.contents[i]; */
		/* 	printf("freeing %d", block.id); */
		/* 	free(block.title); */
		/* 	free(block._class); */
		/* 	free(block.base_class); */
		/* } */

		/* free(c.contents); */
	}

	curl_multi_cleanup(multi_handle);
}

int main(int argc, char **argv) {
  sqlite3 *db = setup_db();
	char *slugs[] = {
			"journal-wandering", 
			"list-websites-lwvz0acnwli",
			"list-are-na-api-possibilities",
			"list-are-na-gists",
			"digital-postcards-postcards-people-make"
	};

	process_multiple_channels(slugs, 5, db);

  listChannel(db);
  sqlite3_close(db);

	return 0;
}

