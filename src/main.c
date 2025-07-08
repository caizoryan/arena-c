#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "channel.h"
#include "sqlite.h"

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

	/* Parse and add channel */
	for(int i = 0; i < requests.len; i++){
		Channel channel = parse_channel(requests.buff[i]); 

		add_channel(db, channel);
		clean_channel(channel);
	}

	curl_multi_cleanup(multi_handle);
}
int main(int argc, char **argv) {
  sqlite3 *db = setup_db();
	char *slugs[] = {
			"notes-unthought",
			"list-are-na-gists",
			/* "notes-xenofeminism-and-planetary-orientation", */
			/* "journal-wandering", */
			/* "list-websites-lwvz0acnwli", */
			/* "list-are-na-api-possibilities", */
			/* "reference-computers-terminal-user-interface", */
			/* "reference-gd1-why-do-you-love-p3", */
			/* "digital-postcards-postcards-people-make" */
	};

	process_multiple_channels(slugs, 2, db);

  list_channel(db);
  sqlite3_close(db);

	return 0;
}

