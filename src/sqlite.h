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
void add_block_connection(sqlite3 *db, Block block);
void add_block(sqlite3 *db, Block block);
void add_user(sqlite3 *db, User user);
void add_image_data(sqlite3 *db, ImageData image, int id);

void mark_block_mounted(sqlite3 *db, int id);
time_t get_block_mounted(sqlite3 *db, int id);

int channel_exists(sqlite3 *db, char* slug);

typedef struct {
	int id;
	char* title;
	char* content;
	char* _class;
} SimpleBlock;

typedef struct {
	int len;
	SimpleBlock* blocks;
} SimpleBlockList;

SimpleBlockList channel_blocks(sqlite3 *db, int id);

#endif
