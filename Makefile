.PHONY: all clean debug
CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS =
DEBUGFLAGS = -O0 -g
SRC = src/sgps.c
BIN = sgps.o

all: sgps

sgps: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS}

debug: ${SRC}

	${CC} -o ${BIN} ${SRC} ${CFLAGS} ${LDFLAGS} ${DEBUGFLAGS}

clean:
	
	rm ${BIN}
