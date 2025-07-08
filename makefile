SOURCES = $(shell ls src/*.c)
build: $(SOURCES)
	clang  -L/opt/homebrew/opt/curl/lib -lsqlite3 -lcurl -lncurses $(SOURCES) -g -O0 -o build 

clean: todo.db
	rm todo.db && touch todo.db && sqlite3 todo.db '.read setup.sql'
