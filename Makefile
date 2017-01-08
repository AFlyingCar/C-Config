CC=gcc
AR=ar

CFLAGS=-Wall -Werror -Wextra -pedantic -ansi

INCLUDES=-I./include/
SOURCES=config

.PHONY: all build

all: build link

build:
	for i in ${SOURCES}; do \
		${CC} ${CFLAGS} ${INCLUDES} -c src/$$i.c -o build/$$i.o; \
	done

link:
	${AR} rcs bin/libconfig.a build/*.o

clean:
	rm build/*.o
	rm bin/config.a

