#ifndef SQLITE
#define SQLITE

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "channel.h"

sqlite3 *setup_db();
sqlite3 *setup_db();

void list_channel(sqlite3 *db);
void add_channel(sqlite3 *db, Channel channel);

int channel_exists(sqlite3 *db, char* slug);

typedef struct {
	int id;
	char* title;
	char* content;
} SimpleBlock;

typedef struct {
	int len;
	SimpleBlock* blocks;
} SimpleBlockList;

SimpleBlockList channel_blocks(sqlite3 *db, int id);

#endif
