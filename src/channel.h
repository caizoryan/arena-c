#ifndef CHANNEL
#define CHANNEL

#include <curl/curl.h>
#include "cJSON.h"
#include "main.h"

typedef struct {
	CURL *curl;
	struct curl_slist *headers;
	struct memory data;
} ChannelRequest;

/* size_t cb(char *data, size_t size, size_t nmemb, struct memory *mem); */
void clean_channel_request(ChannelRequest *req);
typedef struct {
  /* char id[64]; */
  char *slug;
  /* char title[512]; */

  cJSON *data;
} ChannelParsed;

ChannelParsed parse_channel(ChannelRequest *request);
ChannelRequest *construct_channel_request(char *slug);

#endif
