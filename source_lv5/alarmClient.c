#include "stems.h"

void receiveFromDataPost(char* msg)
{
    int fd;
    int length;

    if ((fd = open("./myfifo", O_RDWR)) < 0) {
        printf("fail to call fifo()\n");
    }

    Rio_readn(fd, &length, sizeof(int));
    Rio_readn(fd, msg, length);
    printf(msg);
    Close(fd);

    
}

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

int main(void)
{
    int clientfd;
    char myname[MAXLINE], hostname[MAXLINE], filename[MAXLINE];
    int port;
    float threshold, value;
    char* token;

    char msg[MAXLINE];

    getargs_pc(myname, hostname, port, filename, &threshold);
    receiveFromDataPost(msg);

    

    /* strtok query */
    token = strtok(msg, "="); // name
    token = strtok(NULL, "&"); // name value
    // strcpy(name, token);
    token = strtok(NULL, "="); // time
    token = strtok(NULL, "&"); // time value
    // time = atoi(token);
    token = strtok(NULL, "="); // value
    token = strtok(NULL, "&"); // value value
    value = atof(token);

    if (value > threshold) {
        clientfd = Open_clientfd(hostname, port);
        clientSend(clientfd, filename, msg);
        clientPrint(clientfd);
        Close(clientfd);
    }

}