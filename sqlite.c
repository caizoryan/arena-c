#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

sqlite3 *setup_db();
//   ____  _   _ _____    _    ____  _____ ____  ____  
//  / / / | | | | ____|  / \  |  _ \| ____|  _ \/ ___| 
// / / /  | |_| |  _|   / _ \ | | | |  _| | |_) \___ \ 
/// / /   |  _  | |___ / ___ \| |_| | |___|  _ < ___) |
//_/_/    |_| |_|_____/_/   \_\____/|_____|_| \_\____/ 
void addTodo(sqlite3 *db, char *todo);
void listTodo(sqlite3 *db);

//	__       
//	/ _|_ __  
//	| |_| '_  
//	|  _| | | |
//	|_| |_| |_|

int main(int argc, char **argv){
  sqlite3 *db = setup_db();
  printf("sqlite open ready to use...\n");

  if (argc > 1 ){addTodo(db, argv[1]);}

  listTodo(db);

  sqlite3_close(db);
  return 0;
}
void addTodo(sqlite3 *db, char *todo) {
  char sql[256];
  sprintf(sql, "insert into TODOS(Id, Title, Completed) values(NULL, '%s', 0);", todo);
  printf("%s\n",sql);
  char *err = 0;

  int rc = sqlite3_exec(db, sql, 0, 0, &err);

  if (rc != SQLITE_OK){fprintf(stderr, "ERROR");}
  else {printf("succesfully done\n");}

  free(err);
}

void listTodo(sqlite3 *db) {
  sqlite3_stmt *stmt; 
  const char *sql = "select * from TODOS;";

  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK){
	fprintf(stderr, "ALL HELL HAS BROKEN LOOSE");
  }

  printf("%-5s%-25s%s\n", "id", "title", "status");
  printf("-----------------------------------------------------\n");
  while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
	int id = sqlite3_column_int(stmt, 0);
	char *title = (char *)sqlite3_column_text(stmt, 1);
	int completed = sqlite3_column_int(stmt, 2);
	char *status = "done";
	if(!completed) status = "not";
	printf("%-5d%-25s%s\n", id, title, status);
	printf("-----------------------------------------------------\n");
  }

}
sqlite3 *setup_db(){
  sqlite3* db;
  int rc = sqlite3_open("todo.db", &db);
  if (rc != SQLITE_OK){
	fprintf(stderr, "couldn't open\n");
	return NULL;
  }

  return db;
}
