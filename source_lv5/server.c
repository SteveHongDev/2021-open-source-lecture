#include "stems.h"
#include "request.h"

// 
// To run:
// 1. Edit config-ws.txt with following contents
//    <port number>
// 2. Run by typing executable file
//    ./server
// Most of the work is done within routines written in request.c
//

void getargs_ws(int *port)
{
  FILE *fp;

  if ((fp = fopen("config-ws.txt", "r")) == NULL)
    unix_error("config-ws.txt file does not open.");

  fscanf(fp, "%d", port);
  fclose(fp);
}

void consumer(int connfd, long arrivalTime)
{
  requestHandle(connfd, arrivalTime);
  Close(connfd);
}


int main(void)
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;
  char *argv[] = {NULL};

  initWatch();
  getargs_ws(&port);

  listenfd = Open_listenfd(port);

  int pid = Fork();
  if (pid == 0) {
    Execve("./alarmClient", argv, environ);
  }
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    consumer(connfd, getWatch());
  }
  return(0);
}
