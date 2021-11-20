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

typedef struct Queue {
  int fd;
  int sec;
}Queue;

sem_t* empty;
sem_t* mutex;
sem_t* full;
int poolSize, queueSize;
Queue* queue;
int fill = 0;
int use = 0;
struct timeval now;

void put(int value)
{
  queue[fill].fd = value;
  gettimeofday(&now, NULL);
  queue[fill].sec = now.tv_sec;
  fill = (fill + 1) % queueSize;
}

Queue get()
{
  Queue temp = queue[use];
  use = (use + 1) % queueSize;
  return temp;
}

void getargs_ws(int *port, int *pSize, int *qSize)
{
  FILE *fp;

  if ((fp = fopen("config-ws.txt", "r")) == NULL)
    unix_error("config-ws.txt file does not open.");

  fscanf(fp, "%d", port);
  fscanf(fp, "%d", pSize);
  fscanf(fp, "%d", qSize);
  fclose(fp);
}

void* consumer(void* arg)
{
  while (1) {
    semaphore_wait(full);
    semaphore_wait(mutex);
    Queue enqueue = get();
    semaphore_signal(mutex);
    semaphore_signal(empty);
      
    requestHandle(enqueue.fd, enqueue.sec);
    Close(enqueue.fd);
  }
}


int main(void)
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  getargs_ws(&port, &poolSize, &queueSize);

  if (access("./fifo", F_OK) == 0)
    Unlink("./fifo");

  Mkfifo("./fifo", 0777);

  empty = make_semaphore(queueSize);
  full = make_semaphore(0);
  mutex = make_semaphore(1);

  pthread_t* pthread = (pthread_t*)malloc(sizeof(pthread_t) * poolSize);
  queue = (Queue*)malloc(sizeof(Queue) * queueSize);
  for (int i = 0; i < poolSize; i++) {
    pthread_create(&pthread[i], NULL, consumer, NULL);
  }
  

  listenfd = Open_listenfd(port);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    semaphore_wait(empty);
    semaphore_wait(mutex);
    put(connfd);
    semaphore_signal(mutex);
    semaphore_signal(full);
  }

  

  return(0);
}
