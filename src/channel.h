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
	int id;
	int length;
	int user_id;

	char *status;

	char *slug;
	char *title;
	char *updated_at;
	char *created_at;

} Channel;

Channel parse_channel(ChannelRequest *request);
ChannelRequest *construct_channel_request(char *slug);

#endif
