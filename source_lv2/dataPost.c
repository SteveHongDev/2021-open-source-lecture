#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>

//
// This program is intended to help you test your web server.
// 

int main(int argc, char *argv[])
{

  
  char astr[MAXBUF];
  char *buf;
  /* Print Environment Variable */
  
  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Web Server\r\n");

  buf = Getenv("REQUEST_METHOD");
  printf("Request-Method: %s\n", buf);

  buf = Getenv("CONTENT_LENGTH");
  int contentLength = atoi(buf);
  printf("Content-Length: %d\r\n", contentLength);

  printf("Content-Type: text/plain\r\n\r\n");

  Rio_readn(STDIN_FILENO, astr, contentLength);
  printf(astr);
  
  printf("\n");

  fflush(stdout);
  return(0);
}
