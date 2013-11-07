
CC = g++
CFLAGS = -g -c -Wall
CCF = ${CC} ${CFLAGS}
BINPATH = ../bin

all: client server-multiprocess server-multithread server-iomultiplexing
.PHONY: all


# generate executable file

client: main-client.o client.o util.o
	${CC} main-client.o client.o util.o -o ${BINPATH}/client

server-multiprocess: main-server-multiprocess.o server-multiprocess.o base-server.o session.o util.o
	${CC} main-server-multiprocess.o server-multiprocess.o base-server.o session.o util.o -o ${BINPATH}/server-multiprocess

server-multithread: main-server-multithread.o server-multithread.o base-server.o session.o util.o
	${CC} -pthread main-server-multithread.o server-multithread.o base-server.o session.o util.o -o ${BINPATH}/server-multithread

server-iomultiplexing: main-server-iomultiplexing.o server-iomultiplexing.o base-server.o session.o util.o
	${CC} main-server-iomultiplexing.o server-iomultiplexing.o base-server.o session.o util.o -o ${BINPATH}/server-iomultiplexing

# make client

main-client.o: main-client.cpp
	${CCF} main-client.cpp

client.o: client.cpp
	${CCF} client.cpp



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


# make IO multiplexing server

main-server-iomultiplexing.o: main-server-iomultiplexing.cpp
	${CCF} main-server-iomultiplexing.cpp

server-iomultiplexing.o: server-iomultiplexing.cpp
	${CCF} server-iomultiplexing.cpp



# common part

session.o: session.cpp
	${CCF} session.cpp

base-server.o: base-server.cpp
	${CCF} base-server.cpp

util.o: util.cpp
	${CCF} util.cpp