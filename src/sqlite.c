#include "sqlite.h"
#include "channel.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Add enum return codes for these functions

void add_block_connection(sqlite3 *db, Block block);
void add_block(sqlite3 *db, Block block);
void add_user(sqlite3 *db, User user);
void add_image_data(sqlite3 *db, ImageData image, int id);

void add_channel(sqlite3 *db, Channel channel) {
  char* sql = sqlite3_mprintf(
			"INSERT OR REPLACE INTO channel "
			"(id, title, slug, length, user_id, status, created_at, updated_at) "
      "VALUES "
			"(%d, '%q', %q, %d, %d, %q, %q, %q)",
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

	add_user(db, *channel.user);

  free(err);
}
void add_block(sqlite3 *db, Block block) {
	char* sql = sqlite3_mprintf(
			"INSERT OR REPLACE INTO block "
			"(id, title, class, base_class, content) "
      "VALUES "
			"(%d, '%q', %q, %q, '%q')",
					block.id, block.title, block._class,
					block.base_class, block.content);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR: %d\n %s\n\nSQLITE\n%s\n\n", block.id, err, sql);}
	sqlite3_free(sql);

	if (block.image != NULL) {
		add_image_data(db, *block.image, block.id);
	}

	add_user(db, *block.user);
	add_block_connection(db, block);

  free(err);
}
void add_user(sqlite3 *db, User user) {
	char* sql = sqlite3_mprintf(
			"INSERT OR REPLACE INTO users "
			"(id, slug, username, first_name, last_name, avatar,  created_at, channel_count, following_count, follower_count) "
      "VALUES "
			"(%d, %q, %q, %q, %q, %q, %q, %d, %d, %d)",
			user.id, user.slug, user.username, user.first_name, user.last_name,
			user.avatar, user.created_at, user.channel_count, user.following_count, user.follower_count);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_free(sql);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR: %s\n %s\n\nSQLITE\n%s\n\n", user.slug, err, sql);}
  /* else {printf("succesfully done block\n");} */

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
void add_image_data(sqlite3 *db, ImageData image, int id){
	char* sql = sqlite3_mprintf(
			"INSERT OR REPLACE INTO image_data "
			"(id, filename, content_type, large_url, original_url, display_url, thumb_url, square_url) "
      "VALUES "
			"(%d, '%q', '%q', '%q', '%q', '%q', '%q', '%q')",
					id, image.filename, image.content_type, image.large_url, image.original_url,
					image.display_url, image.thumb_url, image.square_url);

  char *err = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &err);
	sqlite3_free(sql);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR: %d\n %s\n\nSQLITE\n%s\n\n", id, err, sql);}
  /* else {printf("succesfully done block\n");} */

  free(err);

}

int channel_exists(sqlite3 *db, char* slug){
  sqlite3_stmt *stmt; 
  char *sql = sqlite3_mprintf("select * from channel where slug = '%q'", slug);
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  if (rc != SQLITE_OK){
		fprintf(stderr, "ALL HELL HAS BROKEN LOOSE\n");
  }

	int count = 0;

  while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		int id = sqlite3_column_int(stmt, 0);
		count = id;
		char *slug= (char *)sqlite3_column_text(stmt, 2);
		printf("-----------------------------------------------------\n");
		printf("EXISTS FOUND:\n %-5d%-25s\n", id, slug);
		printf("-----------------------------------------------------\n");
  }

	return count;
}


SimpleBlockList channel_blocks(sqlite3 *db, int id){
  sqlite3_stmt *stmt; 

  char *sql = sqlite3_mprintf
		("SELECT block.id, block.title, block.content FROM connections "
		 "INNER JOIN block ON connections.block_id = block.id "
		 "WHERE connections.parent_id = %d", id);

  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  if (rc != SQLITE_OK){
		fprintf(stderr, "ERROr ofc...\n");
  }

	int count = 0;
	SimpleBlockList blocks;

	int malloced = 100;
	blocks.blocks = malloc(sizeof(SimpleBlock) * malloced);

  while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
		int id = sqlite3_column_int(stmt, 0);
		char *title = (char *)sqlite3_column_text(stmt, 1);
		char *content = (char *)sqlite3_column_text(stmt, 2);

		if (id == 31555948) {
			printf("--------------------------");
			printf("PAY ATTENTION");
			printf("--------------------------");
			printf("\n\n%s\n\n", content);
			printf("-------------------------- \n");
		};

		blocks.blocks[count].id = id;
		blocks.blocks[count].title = malloc(strlen(title));
		strcpy(blocks.blocks[count].title, title);

		blocks.blocks[count].content = malloc(strlen(content));
		strcpy(blocks.blocks[count].content, content);

		/* printf("FOUND: %-5d%-25s\n", blocks.blocks[count].id, blocks.blocks[count].title); */
		/* printf("-----------------------------------------------------\n"); */

		count++;
		if (count > malloced) {
			printf("remallocing...\n");
			malloced *= 2;
			SimpleBlock *ptr = realloc(blocks.blocks, sizeof(SimpleBlock)*malloced);
			blocks.blocks = ptr;
		}
		// When count goes above malloced, realloc
  }

	blocks.len = count;
	return blocks;
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
