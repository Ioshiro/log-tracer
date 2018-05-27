# -*- MakeFile -*-

all: log main

log: log.o
	gcc log.o -o run

log.o: log.c
	gcc -c log.c

main: main.o
	gcc main.o -o run

main.o: main.c
	gcc -c main.c

clear:
	rm *.o run
