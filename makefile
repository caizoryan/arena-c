SOURCES = $(shell ls src/*.c)
build: src/main.c
	clang -L/opt/homebrew/opt/curl/lib -lcurl -lncurses $(SOURCES) -o build

