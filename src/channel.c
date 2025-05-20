#include "channel.h"
#include <stdlib.h>
#include <string.h>

ChannelParsed parse_channel(ChannelRequest *request){
    ChannelParsed channel;
    char *parsable = request->data.response;
    cJSON *data = cJSON_Parse(parsable);
    channel.data = data;
    return channel;
}

static size_t cb(char *data, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)userp;

  /* printf("size %ld\n", realsize); */
  /* char *str = malloc(9000); */
  /* strcpy(str, data); */
  /* str[realsize] = '\0'; */
  /* printf("size %ld %s\n", realsize, str); */
  /* free(str); */

   // +1 important for null terminator
  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if(!ptr) return 0;
  /**/
  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
/* // +1 important for null terminate! */
  mem->response[mem->size] = '\0';
 
  return realsize;
}

ChannelRequest construct_channel_request(char *slug){
    CURL *curl = curl_easy_init();

    char url[512] = "https://api.are.na/v2/channels/";
    strcat(url, slug);
    strcat(url, "?per=100");

    struct memory chunk = { 0 };
    struct curl_slist *headers = json_headers();

    ChannelRequest request = {
        .curl = curl,
        .data = chunk,
        .headers = headers,
    };

    if(curl) {
	    // URL & Headers
	    curl_easy_setopt(curl, CURLOPT_URL, url);
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 

	    // handle writing data
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&request.data);
    }


    return request;
}

void clean_channel_request(ChannelRequest req){
    curl_easy_cleanup(req.curl);
    curl_slist_free_all(req.headers);
    free(req.data.response);
}
