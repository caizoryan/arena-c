#include "sqlite.h"
#include "channel.h"
#include <stdio.h>
#include <string.h>

// TODO: Add enum return codes for these functions

void add_block_connection(sqlite3 *db, Block block);
void add_block(sqlite3 *db, Block block);
void add_channel(sqlite3 *db, Channel channel) {
  char sql[2048];
  sprintf(sql,
			"INSERT OR REPLACE INTO channel "
			"(id, title, slug, length, user_id, status, created_at, updated_at) "
      "VALUES "
			"(%d, %s, %s, %d, %d, %s, %s, %s)",
					channel.id, channel.title, channel.slug,
					channel.length, channel.user_id, channel.status,
					channel.created_at, channel.updated_at
					);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR %s", err);}
  /* else {printf("succesfully done\n");} */

	if (channel.contents_len) {
		for (int i = 0; i < channel.contents_len; i++){
			// check if is a block
			if (!strcmp(channel.contents[i].base_class, "\"Block\"")) add_block(db, channel.contents[i]);
		}
	}

  free(err);
}

void add_block(sqlite3 *db, Block block) {
  /* char sql[2048]; */
	char* sql = sqlite3_mprintf(
			"INSERT OR REPLACE INTO block "
			"(id, title, class, base_class, content) "
      "VALUES "
			"(%d, %q, %q, %q, '%q')",
					block.id, block.title, block._class,
					block.base_class, block.content);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_free(sql);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR: %d\n %s\n\nSQLITE\n%s\n\n", block.id,err, sql);}
  /* else {printf("succesfully done block\n");} */
	add_block_connection(db, block);

  free(err);
}

void add_block_connection(sqlite3 *db, Block block){
  char sql[2048];
  sprintf(sql,
			"INSERT OR REPLACE INTO connections "
			"(id, position, parent_id, block_id, channel_id) "
      "VALUES "
			"(%d, %d, %d, %d, %d)",
					block.connection_id, block.position, block.parent_id, block.id, 0);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR: %d\n %s\n\nSQLITE\n%s\n\n", block.id,err, sql);}
  /* else {printf("succesfully done block\n");} */

  free(err);

}

void list_channel(sqlite3 *db) {
  sqlite3_stmt *stmt; 
  const char *sql = "select * from channel;";

  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK){
		fprintf(stderr, "ALL HELL HAS BROKEN LOOSE");
  }

  printf("%-5s%-25s%s\n", "id", "title", "slug");
  printf("-----------------------------------------------------\n");
  while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		int id = sqlite3_column_int(stmt, 0);
		char *title = (char *)sqlite3_column_text(stmt, 1);
		char *slug= (char *)sqlite3_column_text(stmt, 2);
		printf("%-5d%-25s%s\n", id, title, slug);
		printf("-----------------------------------------------------\n");
  }
}
sqlite3 *setup_db() {
  sqlite3* db;
  int rc = sqlite3_open("todo.db", &db);
  if (rc != SQLITE_OK){
		fprintf(stderr, "couldn't open\n");
		return NULL;
  }

  return db;
}
