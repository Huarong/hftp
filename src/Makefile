
CC = g++
CFLAGS = -g -c -Wall
CCF = ${CC} ${CFLAGS}
BINPATH = ../bin

all: client server-multiprocess server-multithread
.PHONY: all


# generate executable file

client: main-client.o client.o util.o
	${CC} main-client.o client.o util.o -o ${BINPATH}/client

server-multiprocess: main-server-multiprocess.o server-multiprocess.o base-server.o session.o util.o
	${CC} main-server-multiprocess.o server-multiprocess.o base-server.o session.o util.o -o ${BINPATH}/server-multiprocess

server-multithread: main-server-multithread.o server-multithread.o base-server.o session.o util.o
	${CC} -pthread main-server-multithread.o server-multithread.o base-server.o session.o util.o -o ${BINPATH}/server-multithread


# make client

main-client.o: main-client.cpp
	${CC} ${CFLAGS} main-client.cpp

client.o: client.cpp
	${CC} ${CFLAGS} client.cpp



# make multiprocess server

main-server-multiprocess.o: main-server-multiprocess.cpp
	${CCF} main-server-multiprocess.cpp

server-multiprocess.o: server-multiprocess.cpp
	${CCF} server-multiprocess.cpp


# make multithread server


main-server-multithread.o: main-server-multithread.cpp
	${CCF} main-server-multithread.cpp

server-multithread.o: server-multithread.cpp
	${CCF} server-multithread.cpp





# common part

session.o: session.cpp
	${CC} ${CFLAGS} session.cpp

base-server.o: base-server.cpp
	${CC} ${CFLAGS} base-server.cpp

util.o: util.cpp
	${CC} ${CFLAGS} util.cpp