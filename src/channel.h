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
	char* slug;
	char* username;
	char* avatar;
	char* first_name;
	char* last_name;
	char* created_at;

	int channel_count;
	int following_count;
	int follower_count;
} User;
typedef struct {
	char* filename;
	char* content_type;
	char* large_url;
	char* original_url;
	char* display_url;
	char* thumb_url;
	char* square_url;
} ImageData;

typedef struct {
	int id;
	int connection_id;
	int position;
	int parent_id;
	char* title;
	char* _class;
	char* base_class;
	char* content;
	User* user;
	ImageData* image;
} Block;

typedef struct {
	int id;
	int length;
	int user_id;

	char* status;

	char* slug;
	char* title;
	char* updated_at;
	char* created_at;

	Block* contents;
	int contents_len;

	User* user;

} Channel;


Channel parse_channel(ChannelRequest *request);
ChannelRequest *construct_channel_request(char *slug);
void clean_channel(Channel channel);

#endif
