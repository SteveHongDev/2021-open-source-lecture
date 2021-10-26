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
  char content[MAXLINE] = "";
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
  float value;
  int flag = 0;

  int id, num;
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

  buf = Getenv("QUERY_STRING");
  
  ptr = strsep(&buf, "=");

  if (!strcmp(ptr, "command")) { // LIST, INFO sname
    ptr = strsep(&buf, "&");
    if (!strcmp(ptr, "LIST")) { // LIST
      mysql_query(conn, "select name from sensorList;");
      res = mysql_store_result(conn);
      row = mysql_fetch_row(res);
      if (row == NULL) {
        sprintf(content, "sensorList is empty.");
      } else {
        sprintf(content, "%s%d. %s\n", content, 1, row[0]);
        for (int i = 2; row = mysql_fetch_row(res); i++) {
          sprintf(content, "%s%d. %s\n", content, i, row[0]);
        }
      }
      
    } else { // INFO
      ptr = strsep(&buf, "=");
      ptr = strsep(&buf, "&");
      sprintf(temp, "select count, average from sensorList where name = '%s';", ptr);
      mysql_query(conn, temp);
      res = mysql_store_result(conn);
      row = mysql_fetch_row(res);
      if (row == NULL) {
        sprintf(content, "There is no sensor named %s.\n", ptr);
      } else {
        sprintf(content, "%ssensor name = %s, count = %s, average = %s\n", content, ptr, row[0], row[1]);
      }
    }
  } else { // GET sname, GET sname n
    ptr = strsep(&buf, "&");
    strcpy(name, ptr);

    ptr = strsep(&buf, "=");
    ptr = strsep(&buf, "&");
    num = atoi(ptr);

    sprintf(temp, "select id, count from sensorList where name = '%s';", name);
    mysql_query(conn, temp);
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    id = atoi(row[0]);
    count = atoi(row[1]);
    for (int i = 0; i < num; i++) {
      sprintf(temp, "select time, value from sensor%d where idx = %d;", id, count - i);
      mysql_query(conn, temp);
      res = mysql_store_result(conn);
      row = mysql_fetch_row(res);
      clock_t time = atol(row[0]);
      char *timeToCtime = ctime(&time);
      timeToCtime[strlen(timeToCtime)-1] = '\0';
      sprintf(content, "%s%d. time = %s, value = %s\n", content, i+1, timeToCtime, row[1]);
    }
    
  }


  
  // /* Generate the HTTP response */
  // printf("Content-Length: %d\n", strlen(content));
  // printf("Content-Type: text/plain\r\n\r\n");
  printf("%s", content);
  fflush(stdout);
}

int main(void)
{
  // htmlReturn();
  textReturn();
  return(0);
}
