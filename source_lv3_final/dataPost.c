#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include </usr/local/include/mysql/mysql.h>

//
// This program is intended to help you test your web server.
// 

void updateDB(char *astr)
{
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

  /* strtok query */
  token = strtok(astr, "="); // name
  token = strtok(NULL, "&"); // name value
  strcpy(name, token);
  token = strtok(NULL, "="); // time
  token = strtok(NULL, "&"); // time value
  time = atoi(token);
  token = strtok(NULL, "="); // value
  token = strtok(NULL, "&"); // value value
  value = atof(token);

  mysql_query(conn, "create table if not exists sensorList (name CHAR(30) NOT NULL, id INT NOT NULL, count INT NOT NULL, average FLOAT NOT NULL,PRIMARY KEY(id));");
  mysql_query(conn, "select * from sensorList;");
  res = mysql_store_result(conn);
  while (row = mysql_fetch_row(res)) { // 0 or pointer
    if (!strcmp(row[0], name)) {
      flag = 1;
      break;
    }
  }

  if (flag == 1) { // Same name found in the sensorList
    /* 1. Update sensorList
      2. Insert to the sensor# list
      3. Free result memory */
      
    // 1
    id = atoi(row[1]);
    count = atoi(row[2]);
    average = (atof(row[3]) * count + value) / ++count;
    sprintf(temp, "update sensorList set count = %d, average = %f where name = \'%s\';", count, average, name);
    mysql_query(conn, temp);

    // 2
    sprintf(temp, "insert into sensor%d(time, value, idx) values(%d, %f, %d);", id, time, value, count);
    mysql_query(conn, temp);

    // 3
    mysql_free_result(res);

    flag = 0;
  } else { // Couldn't find name
    /* 1. Insert to the sensorList
      2. Create sensor# list
      3. Insert to the sensor# list 
      4. Free result memory */

    id = mysql_num_rows(res) + 1;
    
    // 1
    sprintf(temp, "insert into sensorList(name, id, count, average) values(\'%s\', %d, 1, %f);", name, id, value);
    mysql_query(conn, temp);

    // 2
    sprintf(temp, "create table sensor%d (time INT NOT NULL, value FLOAT NOT NULL, idx INT NOT NULL, PRIMARY KEY(idx));", id);
    mysql_query(conn, temp);

    // 3
    sprintf(temp, "insert into sensor%d(time, value, idx) values(%d, %f, 1);", id, time, value);
    mysql_query(conn, temp);

    // 4
    mysql_free_result(res);
  }
}

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

  printf("%s\n", astr);
  fflush(stdout);

  updateDB(astr);  

  return(0);
}
