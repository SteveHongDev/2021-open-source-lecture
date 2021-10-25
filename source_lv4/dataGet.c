#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include </usr/local/include/mysql/mysql.h>

//
// This program is intended to help you test your web server.
// You can use it to test that you are correctly having multiple 
// threads handling http requests.
//
// htmlReturn() is used if client program is a general web client
// program like Google Chrome. textReturn() is used for a client
// program in a embedded system.
//
// Standalone test:
// # export QUERY_STRING="name=temperature&time=3003.2&value=33.0"
// # ./dataGet.cgi

void htmlReturn(void)
{
  char content[MAXLINE];
  char *buf;
  char *ptr;

  /* Make the response body */
  sprintf(content, "%s<html>\r\n<head>\r\n", content);
  sprintf(content, "%s<title>CGI test result</title>\r\n", content);
  sprintf(content, "%s</head>\r\n", content);
  sprintf(content, "%s<body>\r\n", content);
  sprintf(content, "%s<h2>Welcome to the CGI program</h2>\r\n", content);

  buf = Getenv("QUERY_STRING");
  sprintf(content,"%s<p>Env : %s</p>\r\n", content, buf);
  ptr = strsep(&buf, "&");
  while (ptr != NULL){
    sprintf(content, "%s%s\r\n", content, ptr);
    ptr = strsep(&buf, "&");
  }
  sprintf(content, "%s</body>\r\n</html>\r\n", content);
  
  /* Generate the HTTP response */
  printf("Content-Length: %d\r\n", strlen(content));
  printf("Content-Type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
}

void textReturn(void)
{
  char content[MAXLINE];
  char *buf;
  char *ptr;

  char *token;
  MYSQL *conn; // mysql과의 connection 매개체
  MYSQL_RES *res; // query에 대한 result값을 저장하는 위치변수
  MYSQL_ROW row; // row마다 탐색하기 위해 필요한 변수

  char *server = "localhost";
  char *user = "team9";
  char *password = "1234";
  char *database = "opensource";

  char name[MAXLINE];
  int time;
  float value;
  int flag = 0;

  int id;
  int count;
  float average;
  char temp[MAXLINE];

  conn = mysql_init(NULL);

  /* connect to database... */
  if (!mysql_real_connect(conn, server, user, password, NULL, 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(1);
  }
  if (mysql_select_db(conn, database) != 0) {
    mysql_close(conn);
    printf("select_db fail.\n");
  }
  
  /* Generate the HTTP response */
  printf("Content-Length: %d\n", strlen(content));
  printf("Content-Type: text/plain\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
}

int main(void)
{
  // htmlReturn();
  textReturn();
  return(0);
}
