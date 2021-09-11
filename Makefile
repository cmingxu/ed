.PHONY: test clean

CC = gcc

default: build
build: ensure_bin
	${CC} -shared -o bin/libed.so -fPIC -I./include src/ed.c src/config.c src/utils.c -lm
	
build-static: ensure_bin
	${CC} -I./include -o bin/config.o -c src/config.c -lm
	${CC} -I./include -o bin/ed.o -c src/ed.c -lm
	${CC} -I./include -o bin/utils.o -c src/utils.c -lm
	ar rcs bin/libed.a bin/config.o bin/ed.o bin/utils.o
	ranlib bin/libed.a

test: ensure_bin
	${CC} -o bin/test -I./include test/test.c src/ed.c src/config.c src/utils.c -DED_DEBUG

ensure_bin:
	mkdir -p bin
	
clean:
	rm -rf bin > /dev/null 2>&1

