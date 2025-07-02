SOURCES = $(shell ls src/*.c)
build: $(SOURCES)
	clang  -L/opt/homebrew/opt/curl/lib -lsqlite3 -lcurl -lncurses $(SOURCES) -O0 -o build -fsanitize=address -g
