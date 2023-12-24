CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 
STD= -std=c99

all: main

main: main.o
	${CC} main.o -o main ${STD}
	

main.o: main.c
	${CC} ${CFLAGS} main.c ${STD}


clean:
	rm -f *.o *~

