#include "channel.h"
#include "cJSON.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    channel.length = atoi(len);
    channel.id = atoi(id);

		printf("parsing %s\n", channel.slug);
		printf("DONE PARSING %s\n", channel.slug);

		cJSON *contents = cJSON_GetObjectItem(data, "contents");
		int contents_len = cJSON_GetArraySize(contents);
		Block* blocks = malloc(sizeof(Block) * contents_len); 

		for(int i = 0; i < contents_len; i++){
			cJSON *item = cJSON_GetArrayItem(contents, i);
			char *id = cJSON_Print(cJSON_GetObjectItem(item, "id"));

			blocks[i].id = atoi(id);
			blocks[i].title = cJSON_Print(cJSON_GetObjectItem(item, "title"));
			blocks[i]._class = cJSON_Print(cJSON_GetObjectItem(item, "class"));
			blocks[i].base_class = cJSON_Print(cJSON_GetObjectItem(item, "base_class"));

			/* printf("\t%d.\tID:\t%s\t%s\t%s\n", i, id, block.title, block._class); */
			free(id);
		}

		channel.contents = blocks;
		channel.contents_len = contents_len;

		printf("First Block: %s\n", channel.contents[0].title);
		printf("2 Block: %s\n", channel.contents[1].title);
		printf("3 Block: %s\n", channel.contents[2].title);
		printf("4 Block: %s\n", channel.contents[3].title);
		printf("5 Block: %s\n", channel.contents[4].title);

		free(len);
		free(id);

		cJSON_Delete(data);
    return channel;
}


static size_t cb(char *data, size_t size, size_t nmemb, void *userp)
{
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

