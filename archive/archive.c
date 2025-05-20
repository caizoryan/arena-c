#include "channel.h"
 
void filestuff(){
	FILE* file;
	file = fopen("./data.json", "r");
	// get data size using fseek and stuff
	char data[4096];
	int datasize = 0;
	char buff[256];
	int count = 3;

	// use fread instead
	while(fgets(buff, 256, file)){
		//mvprintw(count++, 3, "%s", buff);
		int buffindex = 0;
		char c;
		for(;buffindex < strlen(buff);){ 
			data[datasize++] = buff[buffindex++];
		}

		data[datasize] = '\0';
	}

	cJSON *json = cJSON_Parse(data);
	cJSON *item = cJSON_GetObjectItem(json, "str");
	char *string = cJSON_Print(item);

	fclose(file);
}

