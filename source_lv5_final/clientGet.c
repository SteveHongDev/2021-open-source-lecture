/*
 * clientGet.c: A very, very primitive HTTP client for console.
 * 
 * To run, prepare config-cg.txt and try: 
 *      ./clientGet
 *
 * Sends one HTTP request to the specified HTTP server.
 * Prints out the HTTP response.
 *
 * For testing your server, you will want to modify this client.  
 *
 * When we test your server, we will be using modifications to this client.
 *
 */


#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include "stems.h"

/*
 * Send an HTTP request for the specified file 
 */
void clientSend(int fd, char *filename)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "GET %s HTTP/1.1\n", filename);
  sprintf(buf, "%shost: %s\n\r\n", buf, hostname); // 질문 1
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
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);

    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1) {  //질문 2
      printf("Length = %d\n", length);
    }
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

/* currently, there is no loop. I will add loop later */
void userTask(char hostname[], int port, char webaddr[], int pid)
{
  int clientfd;
  int flag = 1;
  char userRequest[MAXLINE];
  char command[MAXLINE];
  char argument1[MAXLINE];
  char argument2[MAXLINE];
  char *argv[] = {NULL};

  while (flag) {

    printf("#");
    fgets(userRequest, MAXLINE, stdin);
    int userRequestLength = strlen(userRequest);
    if ((userRequestLength > 0) && (userRequest[userRequestLength-1] == '\n'))
      userRequest[userRequestLength-1] = '\0';

    if (strstr(userRequest, " ") == NULL) { // 1 word
      if (!strcmp(userRequest, "LIST")) {
        sprintf(webaddr, "/dataGet.cgi?command=LIST");
        clientfd = Open_clientfd(hostname, port);
        clientSend(clientfd, webaddr);
        clientPrint(clientfd);
        Close(clientfd);
      } else if (!strcmp(userRequest, "QUIT") || !strcmp(userRequest, "EXIT")) {
        flag = 0;
        kill(pid, SIGINT);
      } else {
        printf("Undefined error.\n");
      }
    } else { // 2 or more words
      char *temp = strtok(userRequest, " ");
      strcpy(command, temp);
      temp = strtok(NULL, " ");
      strcpy(argument1, temp);
      if (temp = strtok(NULL, " ")) { // 3 or more words
        strcpy(argument2, temp);
        if (temp = strtok(NULL, " ")) { // more than 3 words
          printf("Too many arguments.\n");
        } else {
          if (!strcmp(command, "GET")) { // 3 words
          sprintf(webaddr, "/dataGet.cgi?NAME=%s&N=%s", argument1, argument2);
          clientfd = Open_clientfd(hostname, port);
          clientSend(clientfd, webaddr);
          clientPrint(clientfd);
          Close(clientfd);
          }
        }
      } else { // 2 words
        if (!strcmp(command, "INFO")) {
          sprintf(webaddr, "/dataGet.cgi?command=INFO&value=%s", argument1);
          clientfd = Open_clientfd(hostname, port);
          clientSend(clientfd, webaddr);
          clientPrint(clientfd);
          Close(clientfd);
        } else if (!strcmp(command, "GET")) {
          sprintf(webaddr, "/dataGet.cgi?NAME=%s&N=1", argument1);
          clientfd = Open_clientfd(hostname, port);
          clientSend(clientfd, webaddr);
          clientPrint(clientfd);
          Close(clientfd);
        } else {
          printf("Undefined error.\n");
        }
      }
    }

  }
}

void getargs_cg(char hostname[], int *port, char webaddr[])
{
  FILE *fp;

  fp = fopen("config-cg.txt", "r");
  if (fp == NULL)
    unix_error("config-cg.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", webaddr);
  fclose(fp);
}

int main(void)
{
  char hostname[MAXLINE], webaddr[MAXLINE];
  int port;
  char *argv[] = {NULL};
  
  getargs_cg(hostname, &port, webaddr);

  int pid = Fork();
  if (pid == 0) {
    Execve("./alarmServer", argv, environ);
  }

  userTask(hostname, port, webaddr, pid);
  
  return(0);
}
