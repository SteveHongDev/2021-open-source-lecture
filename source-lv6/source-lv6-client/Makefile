#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = request.o stems.o clientGet.o clientPost.o clientRPI.o alarmServer.o

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread -lmysqlclient

.SUFFIXES: .c .o 

all: clientPost clientGet clientRPI alarmServer alarm.cgi

clientGet: clientGet.o stems.o
	$(CC) $(CFLAGS) -o clientGet clientGet.o stems.o

clientPost: clientPost.o stems.o
	$(CC) $(CFLAGS) -o clientPost clientPost.o stems.o $(LIBS)

clientRPI: clientRPI.o stems.o
	$(CC) $(CFLAGS) -o clientRPI clientRPI.o stems.o -lwiringPi

alarmServer: alarmServer.o request.o stems.o
	$(CC) $(CFLAGS) -o alarmServer alarmServer.o request.o stems.o $(LIBS)

alarm.cgi: alarm.c stems.h
	$(CC) $(CFLAGS) -o alarm.cgi alarm.c stems.o $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

server.o: stems.h request.h
clientGet.o: stems.h
clientPost.o: stems.h
clientRPI.o: stems.h
alarmServer.o: stems.h request.h

clean:
	-rm -f $(OBJS) server clientPost clientGet clientRPI alarmServer alarm.cgi
