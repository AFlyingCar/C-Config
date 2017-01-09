CC=gcc

CFLAGS=-Wall -Werror -Wextra -pedantic -ansi -g -Wno-unused-variable
OFLAGS=../build/config.o

INCLUDES=-I../include
SOURCES=main

.PHONY: all build

all: build 

build: 
	${CC} ${CFLAGS} ${INCLUDES} ${OFLAGS} main.c -o test

