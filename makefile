SOURCES = $(shell ls src/*.c)
build: $(SOURCES)
	clang  -L/opt/homebrew/opt/curl/lib -lsqlite3 -lcurl -lncurses $(SOURCES) -g -O0 -o build
