CC=gcc
CFLAGS=-O3 -ansi -Wall -lm

build: file.c
	$(CC) -O3 -ansi -Wall -lm -o file file.c

debug: file.c
	$(CC) -g -ansi -Wall -lm -o file file.c

clean:
	rm -f file
