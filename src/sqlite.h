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

#endif
