/*
 * clientPost.c: A very, very primitive HTTP client for sensor
 * 
 * To run, prepare config-cp.txt and try: 
 *      ./clientPost
 *
 * Sends one HTTP request to the specified HTTP server.
 * Get the HTTP response.
 */


#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include "stems.h"

void clientSend(int fd, char *filename, char *body)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "POST %s HTTP/1.1\n", filename);
  sprintf(buf, "%sHost: %s\n", buf, hostname);
  sprintf(buf, "%sContent-Type: text/plain; charset=utf-8\n", buf);
  sprintf(buf, "%sContent-Length: %d\n\r\n", buf, strlen(body));
  sprintf(buf, "%s%s\n", buf, body);
  Rio_writen(fd, buf, strlen(buf));
}
  
/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd)
{
  rio_t rio;
  char buf[MAXBUF];  
  int length = 0;
  int n;
  
  Rio_readinitb(&rio, fd);

  /* Read and display the HTTP Header */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (strcmp(buf, "\r\n") && (n > 0)) {
    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1)
      printf("Length = %d\n", length);
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

typedef struct ThreadArguments {
  char *myname;
  float value;
  char *filename;
  char *hostname;
  int port;
}ThreadArguments;

sem_t mutex;
int num = 0;

void* mythread(void* arg)
{
  int currentTime;
  char msg[MAXLINE];
  char currentCtime[MAXLINE];
  time_t now;
  ThreadArguments *threadArguments = (ThreadArguments *) arg;
  

  float randomTemperature = (float)(rand() % (20 * 10)) / 10 + (threadArguments->value - 10);

  double currentMicroTime;
  struct timeval startTime, endTime;
  currentTime = time(&now);
  sprintf(msg, "name=%s&time=%d&value=%f", threadArguments->myname, currentTime, randomTemperature);

  strcpy(currentCtime, ctime(&now));
  if (currentCtime[strlen(currentCtime)-1] == '\n')
    currentCtime[strlen(currentCtime)-1] = '\0';

  printf("Sending name=%s&time=%s&value=%f ...\n", threadArguments->myname, currentCtime, randomTemperature);

  int clientfd = Open_clientfd(threadArguments->hostname, threadArguments->port);
  
  int thisNum = ++num;

  gettimeofday(&startTime, NULL);
  printf("%d. Current time before sending: %d sec %d microsec\n", thisNum, startTime.tv_sec, startTime.tv_usec);
  
  sem_wait(&mutex);
  clientSend(clientfd, threadArguments->filename, msg);
  clientPrint(clientfd);
  sem_post(&mutex);
  
  gettimeofday(&endTime, NULL);
  printf("%d. Current time after sending: %d sec %d microsec\n", thisNum, endTime.tv_sec, endTime.tv_usec);

  Close(clientfd);
}

/* currently, there is no loop. I will add loop later */
void userTask(char *myname, char *hostname, int port, char *filename, float value)
{
  int clientfd;
  char msg[MAXLINE];
  char userRequest[MAXLINE];
  char command[MAXLINE];
  char argument[MAXLINE];
  char currentCtime[MAXLINE];
  int currentTime;
  time_t now;
  int flag = 1;
  float randomTemperature;

  printf("If you want to see commands, type 'help'\n");
  while (flag) {
    printf(">> ");
    fgets(userRequest, MAXLINE, stdin);
    int userRequestLength = strlen(userRequest);
    if ((userRequestLength > 0) && (userRequest[userRequestLength-1] == '\n'))
      userRequest[userRequestLength-1] = '\0';

    if (strstr(userRequest, " ")) {
      char *temp = strtok(userRequest, " ");
      strcpy(command, temp);
      temp = strtok(NULL, " ");
      strcpy(argument, temp);
      if (strtok(NULL, " ") != NULL) {
        printf("Too many arguments.\n");
        continue;
      }

      if (!strcmp(command, "name")) {
        strcpy(myname, argument);
        printf("Sensor name is changed to '%s'\n", myname);
      } else if (!strcmp(command, "value")) {
        value = atof(argument);
        printf("Sensor value is changed to '%f'\n", value);
      } else if (!strcmp(command, "random")) {
        srand(time(0));
        int repeat = atoi(argument);
        for (int i = 0; i < repeat; i++) {
          // 0.00 ~ 20.00 균등 분포!
          randomTemperature = (float)(rand() % (20 * 100)) / 100 + (value - 10);
          currentTime = time(&now);
          sprintf(msg, "name=%s&time=%d&value=%f", myname, currentTime, randomTemperature);

          strcpy(currentCtime, ctime(&now));
          if (currentCtime[strlen(currentCtime)-1] == '\n')
            currentCtime[strlen(currentCtime)-1] = '\0';

          printf("Sending name=%s&time=%s&value=%f ...\n", myname, currentCtime, randomTemperature);
          clientfd = Open_clientfd(hostname, port);
          clientSend(clientfd, filename, msg);
          clientPrint(clientfd);
          Close(clientfd);
          printf("Sending Completed.\n");
          sleep(1);
        }
      } else if (!strcmp(command, "prand")) {
        
        int repeat = atoi(argument);
        pthread_t threads[repeat];
        ThreadArguments *threadArguments = (ThreadArguments*)malloc(sizeof(ThreadArguments));
        threadArguments->filename = filename;
        threadArguments->hostname = hostname;
        threadArguments->port = port;
        threadArguments->myname = myname;
        threadArguments->value = value;

        srand(time(0));
        for (int i = 0; i < repeat; i++) {
          pthread_create(&threads[i], NULL, mythread, (void *)threadArguments);
        }
        
        for (int j = 0; j < repeat; j++) {
          pthread_join(threads[j], NULL);
        }
        printf("Sending Completed.\n");
        
      } else {
        printf("Undefined Command.\n");
      }
    } else {
      if (!strcmp(userRequest, "help")) {
        printf("help: list available commands.\n");
        printf("name: print current sensor name.\n");
        printf("name <sensor>: change sensor name so <sensor>.\n");
        printf("value: print current value of sensor.\n");
        printf("value <n>: set sensor value to <n>.\n");
        printf("send: send (current sensor name, time, value) to server.\n");
        printf("random <n>: send (name, time, random value) to server <n> times.\n");
        printf("quit: quit the program.\n");
      } else if (!strcmp(userRequest, "name")) {
        printf("Current sensor is '%s'.\n", myname);
      } else if (!strcmp(userRequest, "value")) {
        printf("Current value of sensor is %f.\n", value);
      } else if (!strcmp(userRequest, "send")) {
        currentTime = time(&now);
        sprintf(msg, "name=%s&time=%d&value=%f", myname, currentTime, value);
        strcpy(currentCtime, ctime(&now));
        if (currentCtime[strlen(currentCtime)-1] == '\n')
          currentCtime[strlen(currentCtime)-1] = '\0';
        printf("Sending name=%s&time=%s&value=%f ...\n", myname, currentCtime, value);
        clientfd = Open_clientfd(hostname, port);
        clientSend(clientfd, filename, msg);
        clientPrint(clientfd);
        Close(clientfd);
        printf("Sending Completed.\n");
      } else if (!strcmp(userRequest, "quit")) {
          flag = 0;
      } else {
        printf("Undefined Command.\n");
      }
    }
    
  }
}

void getargs_cp(char *myname, char *hostname, int *port, char *filename, float *value)
{
  FILE *fp;

  fp = fopen("config-cp.txt", "r");
  if (fp == NULL)
    unix_error("config-cp.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%s", myname);
  fscanf(fp, "%f", value);
  fclose(fp);
}

int main(void)
{

  char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE];
  int port;
  float value;
  sem_init(&mutex, 0, 1);

  getargs_cp(myname, hostname, &port, filename, &value);

  userTask(myname, hostname, port, filename, value);
  
  return(0);
}
