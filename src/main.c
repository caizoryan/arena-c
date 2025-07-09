#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

#include "channel.h"
#include "sqlite.h"

struct ChannelRequestArr {
		ChannelRequest *buff[128];
		int len;
} requests;

typedef struct  {
	int len;
	char* list[1024];
} string_list;

string_list read_into_slugs(char* slugs);
string_list process_slugs(char* str);

void process_multiple_channels(char *slugs[], int len, sqlite3 *db){
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
void pull(int argc, char** argv, sqlite3 *db);

/* notes-unthought */
/* list-are-na-gists */
/* journal-wandering */
/* research-convivial-forms */
/* neon-genesis-evangelion-n8th7zgu0w4 */

string_list process_slugs(char* str){
	string_list list = read_into_slugs(str);

	// if len is only one, check if it is a file
	if (list.len == 1) {
		struct stat myFile;
		if (stat(list.list[0], &myFile) < 0) printf("DIR: doesnt exist\n");
		else if (!S_ISDIR(myFile.st_mode)) {
			printf("%s: is a file!\n", list.list[0]);
			FILE* file;

			file = fopen(list.list[0], "r");
			fseek(file, 0, SEEK_END);
			long fsize = ftell(file);
			fseek(file, 0, SEEK_SET);  /* same as rewind(f); */

			char *string = malloc(fsize + 1);
			fread(string, fsize, 1, file);
			fclose(file);

			string[fsize] = '\0' ;
			free(list.list[0]);
			printf("file: %s", string);

			list = read_into_slugs(string);

			free(string);
		}
	}

	return list;
};

void pull(int argc, char** argv, sqlite3 *db){
	char* slugs = argv[2];
	string_list list = process_slugs(slugs);

	printf("PULLING\n");
	for (int i = 0; i < list.len;i++){
		printf("%d. %s\n", i, list.list[i]);
	}

	process_multiple_channels(list.list, list.len, db);

	for (int i = 0; i < list.len;i++){
		free(list.list[i]);
	}
}

int main(int argc, char **argv) {
  sqlite3 *db = setup_db();

	// check if any command
	if (argc > 2) {
		char* cmd = argv[1];

		if (!strcmp(cmd, "pull")){
			pull(argc, argv, db);
		}

		else if (!strcmp(cmd, "list")){
			printf("------------------------------------------------------:\n");
			printf("CHANNELS:\n");
			printf("------------------------------------------------------:\n");
			list_channel(db);
		}

		else if (!strcmp(cmd, "mount")){
			printf("CMD is mount");
			string_list list = process_slugs(argv[2]);
			char* root_dir = "./mount";

			// check if there is a dir supplied
			if (argc > 3) {
				struct stat myFile;
				if (stat(argv[3], &myFile) < 0) printf("%s: Doesnt exist\n", argv[3]);
				else if (!S_ISDIR(myFile.st_mode)) printf("%s: is a file\n", argv[3]);
				else {}
			}

			else {
				printf("NO DIRECTORY, MOUNTING HERE\n");
				printf("------------------------------------------------------:\n\n");
			}

			// ---------------
			// CHECK IF ROOT DIR EXITS
			// ---------------
			struct stat root_dir_check;
			if (stat(root_dir, &root_dir_check) < 0) {
				printf("%s DOESNT exist\n", root_dir);
				int rc = mkdir(root_dir, 0700);
				if (rc < 0) {printf("Error Root Dir %s\n", root_dir);}
			}

			for(int i = 0; i<list.len;i++){
				printf("mounting: \t%s/%s\n", root_dir, list.list[i]);

				// ---------------
				// CHECK IF EXITS IN DB
				// ---------------
				int exists = channel_exists(db, list.list[i]);
				if (exists) {printf("%s, exists\n", list.list[i]);}
				else {printf("\nchannel doesn't exist \n"); continue;}
				
				// ---------------
				// MAKE DIRECTORY
				// ---------------
				// check if dir exists
				struct stat dir_check;
				char dir[2048];
				sprintf(dir, "%s/%s", root_dir, list.list[i]);

				if (stat(argv[3], &dir_check) < 0) {
					int rc = mkdir(dir, 0700);
					if (rc < 0) {
						printf("Error making %s\n", dir);
						continue;
					}
				}

				else {
					printf("%s: DIRECTORY ALREADY EXISTS, CANNOT MOUNT\n", dir);
					continue;
				}

				// get all the blocks
				printf("Getting %d\n", exists);
				SimpleBlockList blocks = channel_blocks(db, exists);

				for (int bb = 0; bb < blocks.len; bb++){
					SimpleBlock block = blocks.blocks[bb];

					if (!block.id) continue;
					if (block.id == 31555948) {
						printf("--------------------------");
						printf("\n\n%s\n\n", block.content);
						printf("-------------------------- \n");
					};
					// check if it is a text block
					char filename[2048];
					sprintf(filename, "%s/%d - %s.txt",dir, block.id, block.title);
					/* printf("Writing: %s\n", filename); */
					FILE* fptr = fopen(filename, "w"); 
					fprintf(fptr, "%s", block.content);
					fclose(fptr);
				}

				// for each text block make a file in format:
				// [id] - [title].md
				// dump all the content into it

				free(list.list[i]);
			}
		}

		else if (!strcmp(cmd, "push")){
			printf("CMD is push");
		}
	}

  /* list_channel(db); */
  sqlite3_close(db);

	return 0;
}

/* char** */
string_list read_into_slugs(char* slugs){
	// go thru the string and read into an array everytime there is a ',' ' ' or '\n'
	string_list list = {.len = 0};
	char buffer[1024];
	int bi = 0;
	int li = 0;
	char c;
	int index = 0;

	while ((c = slugs[index++]) != '\0' ) {
		if (c == ' '
				|| c == ','
				|| c == '\n'
				) {

			if (bi > 0){
				list.list[list.len] = malloc(bi);
				strcpy(list.list[list.len++], buffer);
				bi=0;
			}
		}

		else {
			buffer[bi++] = c;
			buffer[bi] = '\0';
		}

	}

	if (bi > 0) {
		list.list[list.len] = malloc(bi);
		strcpy(list.list[list.len++], buffer);
	}


	return list;
}

// OPTIONS


	/* static struct option long_options[] = { */
	/* 	{"force", no_argument, 0,0}, */
	/* 	{"file", required_argument, 0,0}, */
	/* }; */

	/* int option_index = 0; */
	/* int c; */
	/* while ((c = getopt_long(argc-1, (argv+1), "", long_options, &option_index)) != -1){ */
	/* 	switch (c) { */

	/* 	case 0: */
	/* 		printf("OPT: %s\n", long_options[option_index].name); */
	/* 		if (optarg) printf("ARG: %s\n", optarg); */

	/* 	default:  */
	/* 		printf("END\n"); */
	/* 		break; */
	/* 	} */
	/* } */
