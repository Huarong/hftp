
CC = g++
CFLAGS = -g -c
BINPATH = ../bin

client: main-client.o client.o util.o
	${CC} main-client.o client.o util.o -o ${BINPATH}/client

main-client.o: main-client.cpp
	${CC} ${CFLAGS} main-client.cpp

client.o: client.cpp
	${CC} ${CFLAGS} client.cpp

util.o: util.cpp
	${CC} ${CFLAGS} util.cpp

