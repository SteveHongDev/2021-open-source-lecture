#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o stems.o clientGet.o clientPost.o alarmClient.o alarmServer.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread -lmysqlclient

.SUFFIXES: .c .o 

all: server clientPost clientGet dataGet.cgi dataPost.cgi alarmClient alarmServer alarm.cgi

server: server.o request.o stems.o
	$(CC) $(CFLAGS) -o server server.o request.o stems.o $(LIBS)

clientGet: clientGet.o stems.o
	$(CC) $(CFLAGS) -o clientGet clientGet.o stems.o

clientPost: clientPost.o stems.o
	$(CC) $(CFLAGS) -o clientPost clientPost.o stems.o $(LIBS)

dataGet.cgi: dataGet.c stems.h
	$(CC) $(CFLAGS) -o dataGet.cgi dataGet.c stems.o $(LIBS)

dataPost.cgi: dataPost.c stems.h
	$(CC) $(CFLAGS) -o dataPost.cgi dataPost.c stems.o $(LIBS)

alarmClient: alarmClient.o stems.o
	$(CC) $(CFLAGS) -o alarmClient alarmclient.o stems.o

alarmServer: alarmServer.o request.o stems.o
	$(CC) $(CFLAGS) -o alarmServer alarmServer.o request.o stems.o $(LIBS)

alarm.cgi: alarm.c stems.h
	$(CC) $(CFLAGS) -o alarm.cgi alarm.c stems.o $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

server.o: stems.h request.h
clientGet.o: stems.h
clientPost.o: stems.h
alarmClient.o: stems.h
alarmServer.o: stems.h request.h

clean:
	-rm -f $(OBJS) server clientPost clientGet dataGet.cgi dataPost.cgi alarmClient alarmServer alarm.cgi
