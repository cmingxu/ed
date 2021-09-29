.PHONY: test clean

CC = gcc

default: build
build: ensure_bin
	${CC} -shared -o bin/libed.so -fPIC -I./include src/ed.c src/config.c src/utils.c -lm -DNEW_LOCAL_IP
	
build-static: ensure_bin
	${CC} -I./include -o bin/config.o -c src/config.c -lm
	${CC} -I./include -o bin/ed.o -c src/ed.c -lm
	${CC} -I./include -o bin/utils.o -c src/utils.c -lm
	ar rcs bin/libed.a bin/config.o bin/ed.o bin/utils.o
	ranlib bin/libed.a

happy-path: ensure_bin
	${CC} -o bin/happy-path -I./include test/happy-path.c src/ed.c src/config.c src/utils.c -lm -DED_DEBUG -DNEW_LOCAL_IP

update-new-local-ip: ensure_bin
	${CC} -o bin/update-new-local-ip -I./include test/update-new-local-ip.c src/ed.c src/config.c src/utils.c -lm -DED_DEBUG -DNEW_LOCAL_IP

update-new-device-ip: ensure_bin
	${CC} -o bin/update-new-device-ip -I./include test/update-new-device-ip.c src/ed.c src/config.c src/utils.c -lm -DED_DEBUG -DNEW_LOCAL_IP

connect-tool: ensure_bin
	${CC} -o bin/connect-tool -I./include test/connect-tool.c src/ed.c src/config.c src/utils.c -lm -DED_DEBUG -DNEW_LOCAL_IP

ensure_bin:
	mkdir -p bin
	
clean:
	rm -rf bin > /dev/null 2>&1

