#ifndef CHANNEL
#define CHANNEL

#include <curl/curl.h>
#include "cJSON.h"
#include "main.h"

typedef struct {
	CURL *curl;
	struct curl_slist *headers;
	struct memory data;
	char* body;
} CurlRequest;

typedef struct {
	CURL *curl;
	struct curl_slist *headers;
	struct memory data;
} AddBlockRequest;

/* size_t cb(char *data, size_t size, size_t nmemb, struct memory *mem); */
void clean_channel_request(CurlRequest *req);

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


Channel parse_channel(CurlRequest *request);

CurlRequest *construct_channel_request(char *slug);
CurlRequest *construct_new_block_request(char *channel_slug, char *content);
CurlRequest *construct_update_block_request(int id, char *content);

void clean_channel(Channel channel);

Block* parse_contents(cJSON* channel, int size, int parent_id);
User* parse_user(cJSON* channel);
ImageData* parse_image_data(cJSON* block);
char* json_str_copy(cJSON* json);
void clean_block(Block block);

#endif
