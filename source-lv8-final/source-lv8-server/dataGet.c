#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include </usr/local/include/libbson-1.0/bson/bson.h>
#include </usr/local/include/libmongoc-1.0/mongoc/mongoc.h>

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
  char *database = "opensource";

  char name[MAXLINE];
  float value;
  int flag = 0;

  int num;

  int32_t id;
  int32_t count;
  float average, max;
  char temp[MAXLINE];

  mongoc_client_t *client;
  mongoc_database_t *db;
  mongoc_collection_t *collection;
  mongoc_cursor_t *cursor;

  bson_iter_t iter;
  bson_iter_t iterValue;
  bson_error_t error;
  bson_oid_t oid;

  bson_t *doc;
  bson_t *query;
  bson_t *filter;
  bson_t child;
  char *str;
  mongoc_init();

  /* connect to database... */
  client = mongoc_client_new("mongodb://localhost:27017/");
  db = mongoc_client_get_database(client, database);

  buf = Getenv("QUERY_STRING");
  
  ptr = strtok(buf, "=");

  if (!strcmp(ptr, "command")) { // LIST, INFO sname
    ptr = strtok(NULL, "&");
    if (!strcmp(ptr, "LIST")) { // LIST
      collection = mongoc_client_get_collection(client, database, "sensorList");
      filter = bson_new();
      bson_append_int32(filter, "id", -1, 1);
      cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);
      
      if (!mongoc_cursor_next(cursor, &doc)) {
        sprintf(content, "sensorList is empty.\n");
      } else {
        bson_iter_init(&iter, doc);
        bson_iter_find_descendant(&iter, "name", &iterValue);
        sprintf(content, "%s%d. %s\n", content, 1, bson_iter_utf8(&iterValue, NULL));
        filter = bson_new();
        bson_append_int32(filter, "id", -1, 2);
        cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);
        for (int i = 2; mongoc_cursor_next(cursor, &doc); i++) {
          bson_iter_init(&iter, doc);
          bson_iter_find_descendant(&iter, "name", &iterValue);
          sprintf(content, "%s%d. %s\n", content, i, bson_iter_utf8(&iterValue, NULL));
          filter = bson_new();
          bson_append_int32(filter, "id", -1, i + 1);
          cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);
        }
      }
      
    } else { // INFO
      ptr = strtok(NULL, "=");
      ptr = strtok(NULL, "&");
      collection = mongoc_client_get_collection(client, database, "sensorList");
      filter = bson_new();
      bson_append_utf8(filter, "name", -1, ptr, -1);
      cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

      if (!mongoc_cursor_next(cursor, &doc)) {
        sprintf(content, "There is no sensor named %s.\n", ptr);
      } else {
        bson_iter_init(&iter, doc);
        bson_iter_find_descendant(&iter, "count", &iterValue);
        int tmpCount = bson_iter_int32(&iterValue);
        bson_iter_init(&iter, doc);
        bson_iter_find_descendant(&iter, "average", &iterValue);
        float tmpAverage = bson_iter_double(&iterValue);
        sprintf(content, "%ssensor name = %s, count = %d, average = %f\n", content, ptr, tmpCount, tmpAverage);
      }
    }
  } else { // GET sname, GET sname n
    ptr = strtok(NULL, "&");
    strcpy(name, ptr);

    ptr = strtok(NULL, "=");
    ptr = strtok(NULL, "&");
    num = atoi(ptr);

    collection = mongoc_client_get_collection(client, database, "sensorList");
    filter = bson_new();
    bson_append_utf8(filter, "name", -1, name, -1);
    cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

    if (!mongoc_cursor_next(cursor, &doc)) {
        sprintf(content, "There is no sensor named %s.\n", name);
      } else {
        bson_iter_init(&iter, doc);
        bson_iter_find_descendant(&iter, "id", &iterValue);
        id = bson_iter_int32(&iterValue);
        bson_iter_init(&iter, doc);
        bson_iter_find_descendant(&iter, "count", &iterValue);
        count = bson_iter_int32(&iterValue);
        for (int i = num - 1; i >= 0; i--) {
          sprintf(temp, "sensor%d", id);
          collection = mongoc_client_get_collection(client, database, temp);
          filter = bson_new();
          bson_append_int32(filter, "idx", -1, count - i);
          cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

          mongoc_cursor_next(cursor, &doc);
          bson_iter_init(&iter, doc);
          bson_iter_find_descendant(&iter, "time", &iterValue);

          clock_t time = bson_iter_int32(&iterValue);
          char *timeToCtime = ctime(&time);
          timeToCtime[strlen(timeToCtime)-1] = '\0';

          bson_iter_init(&iter, doc);
          bson_iter_find_descendant(&iter, "value", &iterValue);

          sprintf(content, "%s%d. time = %s, value = %f\n", content, num - i, timeToCtime, bson_iter_double(&iterValue));
        }
      }
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
