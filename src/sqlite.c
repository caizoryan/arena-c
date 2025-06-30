#include "sqlite.h"

void addChannel(sqlite3 *db, Channel channel) {
  char sql[2048];
  sprintf(sql,
			"INSERT OR REPLACE INTO channel "
			"(id, title, slug, length, user_id, status, created_at, updated_at) "
      "VALUES "
			"(%d, '%s', '%s', %d, %d, '%s', '%s', '%s')",
					channel.id, channel.title, channel.slug,
					channel.length, channel.user_id, channel.status,
					channel.created_at, channel.updated_at
					);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR %s", err);}
  else {printf("succesfully done\n");}

  free(err);
} 

void listChannel(sqlite3 *db) {
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
