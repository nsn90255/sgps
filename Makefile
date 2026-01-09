.PHONY: all
CC = gcc
CFLAGS = -Wall -pedantic -std=c23
LDFLAGS =
DEBUGFLAGS = -O0 -g -Werror -DDEBUG -fsanitize=address
SRC = src/sgps.c
BIN = sgps.o

all: sgps

sgps: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS}

debug: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS} ${DEBUGFLAGS}

clean:
	
	rm ${BIN}
