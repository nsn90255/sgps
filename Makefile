.PHONY: all
CC = gcc
CFLAGS = -Wall -pedantic -std=c23
LDFLAGS =
DEBUGFLAGS = -O0 -g -Werror -DDEBUG
SRC = src/sgps.c
BIN = sgps.o
VALFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all

all: sgps

sgps: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS}

debug: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS} ${DEBUGFLAGS}
test: 
	valgrind ${VALFLAGS} ./sgps.o -r testing_server/ -d 127.0.0.1 -p 7000 

clean:
	
	rm ${BIN}
