.PHONY: test clean

CC = gcc

default: build
build: ensure_bin
	${CC} -shared -o bin/libed.so -fPIC -I./include src/ed.c src/config.c src/utils.c
	
test: ensure_bin
	${CC} -o bin/test -I./include test/test.c src/ed.c src/config.c src/utils.c -DED_DEBUG

ensure_bin:
	mkdir -p bin
	
clean:
	rm -rf bin > /dev/null 2>&1

