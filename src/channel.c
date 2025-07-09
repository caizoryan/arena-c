#include "channel.h"
#include "cJSON.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"

Block* parse_contents(cJSON* channel, int size, int parent_id);
User* parse_user(cJSON* channel);
ImageData* parse_image_data(cJSON* block);

Channel parse_channel(ChannelRequest *request){
    Channel channel;
    char *parsable = request->data.response;
    cJSON *data = cJSON_Parse(parsable);

		// TODO: handle cJSON errors
		// TODO: make sure data parsed properly
		// TODO: Remove quotes from cJSON_Print when processing
    channel.slug = cJSON_Print(cJSON_GetObjectItem(data, "slug"));
    channel.title = cJSON_Print(cJSON_GetObjectItem(data, "title"));
    channel.status = cJSON_Print(cJSON_GetObjectItem(data, "status"));
    channel.updated_at = cJSON_Print(cJSON_GetObjectItem(data, "updated_at"));
    channel.created_at = cJSON_Print(cJSON_GetObjectItem(data, "created_at"));

		char *len = cJSON_Print(cJSON_GetObjectItem(data, "length"));
		char *id = cJSON_Print(cJSON_GetObjectItem(data, "id"));
		char *user_id = cJSON_Print(cJSON_GetObjectItem(data, "user_id"));

    channel.id = atoi(id);
    channel.user_id = atoi(user_id);
    channel.length = atoi(len);

		free(len);
		free(id);
		free(user_id);

		cJSON *contents = cJSON_GetObjectItem(data, "contents");
		int size = cJSON_GetArraySize(contents);

		channel.contents = parse_contents(data, size, channel.id);
		channel.contents_len = size;

		channel.user = parse_user(data);

		cJSON_Delete(data);
    return channel;
}
Block* parse_contents(cJSON* channel, int size, int parent_id){
		cJSON *contents = cJSON_GetObjectItem(channel, "contents");
		int contents_len = size;
		Block* blocks = malloc(sizeof(Block) * contents_len); 

		for(int i = 0; i < contents_len; i++){
			cJSON *item = cJSON_GetArrayItem(contents, i);
			char *id = cJSON_Print(cJSON_GetObjectItem(item, "id"));

			blocks[i].id = atoi(id);
			free(id);

			blocks[i].title = cJSON_Print(cJSON_GetObjectItem(item, "title"));
			blocks[i]._class = cJSON_Print(cJSON_GetObjectItem(item, "class"));
			blocks[i].base_class = cJSON_Print(cJSON_GetObjectItem(item, "base_class"));

			cJSON* content_json = cJSON_GetObjectItem(item, "content");
			char* content;
			if (cJSON_IsString(content_json)){
				content = content_json->valuestring;
				int len = strlen(content);
				blocks[i].content = malloc(len);
				strcpy(blocks[i].content, content);
			}

			else {
				blocks[i].content = malloc(3);
				strcpy(blocks[i].content, "");
			}

			blocks[i].user = parse_user(item);
			blocks[i].image = parse_image_data(item);

			char *connection_id = cJSON_Print(cJSON_GetObjectItem(item, "connection_id"));
			blocks[i].connection_id = atoi(connection_id);
			free(connection_id);

			char *position = cJSON_Print(cJSON_GetObjectItem(item, "position"));
			blocks[i].position = atoi(position);
			free(position);

			blocks[i].parent_id = parent_id;
		}

		return blocks;
}
User* parse_user(cJSON* channel){
	cJSON* user_json = cJSON_GetObjectItem(channel, "user");
	User* user = malloc(sizeof(User));

	char *id = cJSON_Print(cJSON_GetObjectItem(user_json, "id"));
	char *channel_count = cJSON_Print(cJSON_GetObjectItem(user_json, "channel_count"));
	char *following = cJSON_Print(cJSON_GetObjectItem(user_json, "following_count"));
	char *follower = cJSON_Print(cJSON_GetObjectItem(user_json, "follower_count"));

	user->channel_count = atoi(channel_count);
	user->id = atoi(id);
	user->following_count = atoi(following);
	user->follower_count = atoi(follower);

	free(follower);
	free(following);
	free(channel_count);
	free(id);

	user->slug = cJSON_Print(cJSON_GetObjectItem(user_json, "slug"));
	user->username = cJSON_Print(cJSON_GetObjectItem(user_json, "username"));
	user->avatar = cJSON_Print(cJSON_GetObjectItem(user_json, "avatar"));
	user->first_name = cJSON_Print(cJSON_GetObjectItem(user_json, "first_name"));
	user->last_name = cJSON_Print(cJSON_GetObjectItem(user_json, "last_name"));
	user->created_at = cJSON_Print(cJSON_GetObjectItem(user_json, "created_at"));

	return user;
}

void clean_block(Block block);
ImageData* parse_image_data(cJSON* block){
	cJSON* image_json = cJSON_GetObjectItem(block, "image");
	ImageData* image;

	if (cJSON_IsNull(image_json) || image_json == NULL) {
		image = NULL;
	}

	else {
		image = malloc(sizeof(ImageData));
		image->content_type = cJSON_Print(cJSON_GetObjectItem(image_json, "content_type"));
		image->filename = cJSON_Print(cJSON_GetObjectItem(image_json, "filename"));
		image->display_url = cJSON_Print(cJSON_GetObjectItem(cJSON_GetObjectItem(image_json, "display"), "url"));
		image->thumb_url = cJSON_Print(cJSON_GetObjectItem(cJSON_GetObjectItem(image_json, "thumb"), "url"));
		image->original_url = cJSON_Print(cJSON_GetObjectItem(cJSON_GetObjectItem(image_json, "original"), "url"));
		image->square_url = cJSON_Print(cJSON_GetObjectItem(cJSON_GetObjectItem(image_json, "square"), "url"));
		image->large_url = cJSON_Print(cJSON_GetObjectItem(cJSON_GetObjectItem(image_json, "large"), "url"));
	}

	return image;
}

static size_t cb(char *data, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)userp;

   // +1 important for null terminator
  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if(!ptr) return 0;

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;

  // +1 important for null terminate!
  mem->response[mem->size] = '\0';
 
  return realsize;
}

ChannelRequest *construct_channel_request(char *slug){
    CURL *curl = curl_easy_init();

    char url[512] = "https://api.are.na/v2/channels/";
    strcat(url, slug);
    strcat(url, "?per=100");

    struct memory chunk = { 0 };
    struct curl_slist *headers = json_headers();

    ChannelRequest *request = malloc(sizeof(ChannelRequest));
    request->curl = curl;
    request->data = chunk;
    request->headers = headers;

    if(curl) {
	    // URL & Headers
	    curl_easy_setopt(curl, CURLOPT_URL, url);
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

	    // handle writing data
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&(request->data));
    }


    return request;
}

void clean_channel_request(ChannelRequest *req){
    curl_easy_cleanup(req->curl);
    curl_slist_free_all(req->headers);
    free(req->data.response);
}
void clean_channel(Channel channel){
		free(channel.slug);
		free(channel.title);
		free(channel.updated_at);
		free(channel.created_at);
		free(channel.status);

		for(int i = 0; i < channel.contents_len; i++){
			Block block = channel.contents[i];
			clean_block(block);
		}
		free(channel.contents);

		free(channel.user->username);
		free(channel.user->avatar);
		free(channel.user->created_at);
		free(channel.user->slug);
		free(channel.user->first_name);
		free(channel.user->last_name);
		free(channel.user);
}

void clean_block(Block block){
		free(block.title);
		free(block._class);
		free(block.base_class);
		free(block.content);

		free(block.user->username);
		free(block.user->avatar);
		free(block.user->created_at);
		free(block.user->slug);
		free(block.user->first_name);
		free(block.user->last_name);
		free(block.user);

		if (block.image != NULL) {
			free(block.image->content_type);
			free(block.image->filename);
			free(block.image->display_url);
			free(block.image->thumb_url);
			free(block.image->large_url);
			free(block.image->original_url);
			free(block.image->square_url);
			free(block.image);
		}
}
