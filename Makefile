.PHONY: all clean debug
CC = gcc
CFLAGS = -Wall -pedantic
LDFLAGS =
DEBUGFLAGS = -O0 -g -Werror
SRC = src/sgps.c
BIN = sgps.o

all: sgps

sgps: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS}

debug: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS} ${DEBUGFLAGS}

clean:
	
	rm ${BIN}
