.PHONY: test clean

CC = gcc

build:
	echo "1"
	
test: ensure_bin
	${CC} -o bin/test -I./include test/test.c src/ed.c src/config.c src/util.c

ensure_bin:
	mkdir -p bin
	
clean:
	rm -rf bin > /dev/null 2>&1

