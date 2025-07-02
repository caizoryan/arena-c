#ifndef SQLITE
#define SQLITE

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "channel.h"

sqlite3 *setup_db();
sqlite3 *setup_db();

void listChannel(sqlite3 *db);
void addChannel(sqlite3 *db, Channel channel);
void cleanChannel(Channel channel);

#endif
