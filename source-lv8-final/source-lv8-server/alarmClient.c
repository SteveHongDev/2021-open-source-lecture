#include "stems.h"

void getargs_pc(char *myname, char *hostname, int *port, char *filename, float *threshold)
{
  FILE *fp;

  fp = fopen("config-pc.txt", "r");
  if (fp == NULL)
    unix_error("config-pc.txt file does not open.");

  fscanf(fp, "%s", hostname);
  fscanf(fp, "%d", port);
  fscanf(fp, "%s", filename);
  fscanf(fp, "%s", myname);
  fscanf(fp, "%f", threshold);
  fclose(fp);
}

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

int checkIfPipeExists(const char *pipename)
{
  FILE *file;
  if (file = fopen(pipename, "r")) {
    fclose(file);
    return 1;
  }
  return 0;
}

int main(void)
{
    int clientfd, fifofd;
    char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE], name[MAXLINE];
    int port;
    float threshold, value;
    char* token;

    int msgSize;
    char msg[MAXLINE];
    char sendmsg[MAXLINE];

    getargs_pc(myname, hostname, &port, filename, &threshold);

    fifofd = Open("./fifo", O_RDWR, 0);
    Read(fifofd, &msgSize, sizeof(int));
    Read(fifofd, msg, msgSize);
    strcpy(sendmsg, msg);

    /* strtok query */
    token = strtok(msg, "="); // name
    token = strtok(NULL, "&"); // name value
    strcpy(name, token);
    token = strtok(NULL, "="); // time
    token = strtok(NULL, "&"); // time value
    // time = atoi(token);
    token = strtok(NULL, "="); // value
    token = strtok(NULL, "&"); // value value
    value = atof(token);

    if ((strstr(name, myname) != NULL) && (value > threshold)) {
        clientfd = Open_clientfd(hostname, port);
        clientSend(clientfd, filename, sendmsg);
        clientPrint(clientfd);
        Close(clientfd);
    }

}
