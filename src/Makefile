
CC = g++
CFLAGS = -g -c
BINPATH = ../bin

client: main.o client.o util.o
	${CC} main.o client.o util.o -o ${BINPATH}/client

main.o: main.cpp
	${CC} ${CFLAGS} main.cpp

client.o: client.cpp
	${CC} ${CFLAGS} client.cpp

util.o: util.cpp
	${CC} ${CFLAGS} util.cpp

