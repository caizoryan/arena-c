#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <dirent.h> 

#include "cJSON.h"
#include "channel.h"
#include "sqlite.h"

struct ChannelRequestArr {
		CurlRequest *buff[128];
		int len;
} requests;

typedef struct  {
	int len;
	char* list[1024];
} string_list;

char* root_dir = "./mount";

string_list read_into_slugs(char* slugs);
string_list process_slugs(char* str);

void pull(int argc, char** argv, sqlite3 *db);
void mount(int argc, char** argv, sqlite3 *db);
void check(int argc, char** argv, sqlite3 *db);
void check_channel_modified(sqlite3 *db, char* location);

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
void mount(int argc, char** argv, sqlite3 *db){
	printf("CMD is mount");
	string_list list = process_slugs(argv[2]);
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
		// ---------------
		// CHECK IF EXITS IN DB
		// ---------------
		int channel_id = channel_exists(db, list.list[i]);
		if (!channel_id) {printf("\nchannel doesn't exist \n"); goto free;}

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
				goto free;
			}
		}

		else {
			printf("%s: DIRECTORY ALREADY EXISTS, CANNOT MOUNT\n", dir);
			goto free;
		}

		// get all the blocks

		SimpleBlockList blocks = channel_blocks(db, channel_id);
		for (int bb = 0; bb < blocks.len; bb++){
			SimpleBlock block = blocks.blocks[bb];

			if (!block.id) goto free;
			if (block.id == 31555948) {
				printf("--------------------------");
				printf("\n\n%s\n\n", block.content);
				printf("-------------------------- \n");
			};

			// check if it is a text block
			if (!strcmp(block._class, "Text")) {
				// for each text block make a file in format:
				// [id] - [title].md

				char filename[2048];
				sprintf(filename, "%s/%d - %s.txt", dir, block.id, block.title);
				FILE* fptr = fopen(filename, "w"); 
				fprintf(fptr, "%s", block.content);
				mark_block_mounted(db, block.id);
				fclose(fptr);
			} 
		}

	free:
		free(list.list[i]);
	}
}

void check_channel_modified(sqlite3 *db, char* location) {
	DIR *d;
  struct dirent *dir;
  d = opendir(location);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
			// ignore first two entries (., ..)
			if (dir->d_type == DT_REG) {

				char file[4096]; 
				strcpy(file, location);
				strcat(file, "/");
				strcat(file, dir->d_name);

				// probably sanitize? see if in correct format...
				int block_id = atoi(dir->d_name);

				struct stat fileptr;
				if (stat(file, &fileptr) == 0) {
					time_t t = fileptr.st_mtime;
					time_t mod = get_block_mounted(db, block_id);
					if (t != mod) { printf("%d: \t modified\n", block_id); }
				}
			}
    }

    closedir(d);
  }
}

typedef struct {
	int id;
	char* content;
} UpdateData;

typedef struct {
	UpdateData* blocks;
	int len;
} UpdateList;

UpdateList get_channel_modified_blocks(sqlite3 *db, char* location) {
	DIR *d;
  struct dirent *dir;
  d = opendir(location);
	UpdateList list;
	int malloced = 5;
	list.blocks = malloc(sizeof(UpdateData) * malloced);
	list.len = 0;

  if (d) {
    while ((dir = readdir(d)) != NULL) {
			// ignore first two entries (., ..)
			if (dir->d_type == DT_REG) {

				char file[4096]; 
				strcpy(file, location);
				strcat(file, "/");
				strcat(file, dir->d_name);

				// probably sanitize? see if in correct format...
				int block_id = atoi(dir->d_name);

				struct stat fileptr;
				if (stat(file, &fileptr) == 0) {
					time_t t = fileptr.st_mtime;
					time_t mod = get_block_mounted(db, block_id);
					if (t != mod) {
						printf("%d: \t %s\t modified\n", block_id, location);

						FILE* file_content;

						file_content = fopen(file, "r");
						fseek(file_content, 0, SEEK_END);
						long fsize = ftell(file_content);
						fseek(file_content, 0, SEEK_SET);  /* same as rewind(f); */

						char *string = malloc(fsize + 1);
						fread(string, fsize, 1, file_content);
						fclose(file_content);

						string[fsize] = '\0' ;
						printf("CONTENT:   %s\n\n", string);
						list.blocks[list.len].id = block_id;
						list.blocks[list.len++].content = string;

						if (malloced == list.len) {
							malloced *= 2;
							UpdateData *ptr = realloc(list.blocks, sizeof(SimpleBlock)*malloced);
							list.blocks = ptr;
						}
					}
				}
			}
    }

    closedir(d);
  }
	return list;
}

void check(int argc, char** argv, sqlite3 *db){
	DIR *d;
  struct dirent *dir;

	/* char* slugs = argv[2]; */
	/* string_list list = process_slugs(slugs); */

  d = opendir(root_dir);
	int ignore = 2;

  if (d) {
    while ((dir = readdir(d)) != NULL) {
			// ignore first two entries (., ..)
			if (ignore) {ignore--;continue;}

			if (dir->d_type == DT_DIR) {
				// only if dir->d_name is in list
				char location[4096]; 
				strcpy(location, root_dir);
				strcat(location, "/");
				strcat(location, dir->d_name);

				check_channel_modified(db, location);
			}
			/* else printf("%s\n", dir->d_name); */
    }

    closedir(d);
  }

}
void update(int argc, char** argv, sqlite3 *db){
	DIR *d;
  struct dirent *dir;

	/* char* slugs = argv[2]; */
	/* string_list list = process_slugs(slugs); */

  d = opendir(root_dir);
	int ignore = 2;

  if (d) {
    while ((dir = readdir(d)) != NULL) {
			// ignore first two entries (., ..)
			if (ignore) {ignore--;continue;}

			if (dir->d_type == DT_DIR) {
				// only if dir->d_name is in list
				char location[4096]; 
				strcpy(location, root_dir);
				strcat(location, "/");
				strcat(location, dir->d_name);

				UpdateList block_list = get_channel_modified_blocks(db, location);
				printf("TO UPDATE: \n");

				for (int i = 0; i < block_list.len; i++){
					printf("Block: %d\nContent: \n%s\n", block_list.blocks[i].id, block_list.blocks[i].content);

					CurlRequest *req = construct_update_block_request(block_list.blocks[i].id, block_list.blocks[i].content);
					int res = curl_easy_perform(req->curl);
					clean_channel_request(req);

					cJSON *data = cJSON_Parse(req->data.response);
					cJSON_Delete(data);
					free(block_list.blocks[i].content);
					// then when success, fetch the block and update sqlite!
				}

				free(block_list.blocks);
			}
    }

    closedir(d);
  }

}

int main(int argc, char **argv) {
  sqlite3 *db = setup_db();

	// check if any command
	if (argc > 2) {
		char* cmd = argv[1];

		if (!strcmp(cmd, "list")){
			printf("------------------------------------------------------:\n");
			printf("CHANNELS:\n");
			printf("------------------------------------------------------:\n");
			list_channel(db);
		}

		else if (!strcmp(cmd, "pull")) pull(argc, argv, db);
		else if (!strcmp(cmd, "mount")) mount(argc, argv, db);
		else if (!strcmp(cmd, "check")) check(argc, argv, db);
		else if (!strcmp(cmd, "update")) update(argc, argv, db);
		else if (!strcmp(cmd, "push")) update(argc, argv, db);
	}
	else {
		CurlRequest *req = construct_new_block_request("test-ejghk6g7dyc", "testing if this shit works");
		int res = curl_easy_perform(req->curl);

    cJSON *data = cJSON_Parse(req->data.response);
    cJSON *block_id = cJSON_GetObjectItem(data, "id");
    int id = block_id->valueint;

		CurlRequest *req2 = construct_update_block_request(id, "ass fucker!");
		res = curl_easy_perform(req2->curl);
	}

  /* list_channel(db); */
  sqlite3_close(db);

	return 0;
}

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
