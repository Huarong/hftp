
CC = g++
CFLAGS = -g -c -Wall
CCF = ${CC} ${CFLAGS}
BINPATH = ../bin

# make client

# client: main-client.o client.o util.o
# 	${CC} main-client.o client.o util.o -o ${BINPATH}/client

# main-client.o: main-client.cpp
# 	${CC} ${CFLAGS} main-client.cpp

# client.o: client.cpp
# 	${CC} ${CFLAGS} client.cpp



# make multiprocess server

server-multiprocess: main-server-multiprocess.o server-multiprocess.o base-server.o util.o
	${CC} main-server-multiprocess.o server-multiprocess.o base-server.o util.o -o ${BINPATH}/server-multiprocess

main-multiprocess.o: main-server-multiprocess.cpp
	${CCF} main-server-multiprocess.cpp

server-multiprocess.o: server-multiprocess.cpp
	${CCF} server-multiprocess.cpp

base-server.o: base-server.cpp
	${CC} ${CFLAGS} base-server.cpp




# common part

util.o: util.cpp
	${CC} ${CFLAGS} util.cpp