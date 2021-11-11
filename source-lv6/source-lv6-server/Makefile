#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o stems.o alarmClient.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread -lmysqlclient

.SUFFIXES: .c .o 

all: server dataGet.cgi dataPost.cgi alarmClient

server: server.o request.o stems.o
	$(CC) $(CFLAGS) -o server server.o request.o stems.o $(LIBS)

dataGet.cgi: dataGet.c stems.h
	$(CC) $(CFLAGS) -o dataGet.cgi dataGet.c stems.o $(LIBS)

dataPost.cgi: dataPost.c stems.h
	$(CC) $(CFLAGS) -o dataPost.cgi dataPost.c stems.o $(LIBS)

alarmClient: alarmClient.o stems.o
	$(CC) $(CFLAGS) -o alarmClient alarmClient.o stems.o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

server.o: stems.h request.h
alarmClient.o: stems.h

clean:
	-rm -f $(OBJS) server dataGet.cgi dataPost.cgi alarmClient
