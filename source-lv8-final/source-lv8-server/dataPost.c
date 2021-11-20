#include "stems.h"
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include </usr/local/include/libbson-1.0/bson/bson.h>
#include </usr/local/include/libmongoc-1.0/mongoc/mongoc.h>

//
// This program is intended to help you test your web server.
// 


void updateDB(char *astr)
{
  char *token;
  char *database = "opensource";

  char name[MAXLINE];
  int time;
  float value;
  int flag = 0;

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

  collection = mongoc_client_get_collection(client, database, "sensorList");
  filter = bson_new();
  bson_append_utf8(filter, "name", -1, name, -1);
  cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

  if (mongoc_cursor_next(cursor, &doc)) { // Same name found in the sensorList
    /* 1. Update sensorList
      2. Insert to the sensor# list */
    // 1

    bson_iter_init(&iter, doc);

    bson_iter_find_descendant(&iter, "id", &iterValue);
    id = bson_iter_int32(&iterValue);
    bson_iter_find_descendant(&iter, "count", &iterValue);
    count = bson_iter_int32(&iterValue);
    bson_iter_find_descendant(&iter, "average", &iterValue);
    average = (bson_iter_double(&iterValue) * count + value) / ++count;
    sprintf(temp, "%.2f", average);
    average = atof(temp);
    
    query = BCON_NEW("name", BCON_UTF8(name));

    doc = BCON_NEW(
      "$set",
      "{",
        "count", BCON_INT32(count),
        "average", BCON_DOUBLE(average),
      "}"
    );

    if (!mongoc_collection_update_one(collection, query, doc, NULL, NULL, &error)) {
      fprintf(stderr, "%s\n", error.message);
    }


    // 2
    sprintf(temp, "sensor%d", id);
    collection = mongoc_client_get_collection(client, database, temp);

    bson_oid_init(&oid, NULL);
    doc = BCON_NEW("_id", BCON_OID(&oid), "time", BCON_INT32(time), "value", BCON_DOUBLE(value), "idx", BCON_INT32(count));
    if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, &error)) {
      fprintf(stderr, "%s\n", error.message);
    }

  } else { // Couldn't find name
    /* 1. Insert to the sensorList
      2. Create sensor# list
      3. Insert to the sensor# list */

    // 1
    collection = mongoc_client_get_collection(client, database, "sensorList");
    filter = bson_new();
    id = mongoc_collection_count_documents(collection, filter, NULL, NULL, NULL, NULL) + 1;

    doc = bson_new();
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(doc, "_id", &oid);
    BSON_APPEND_UTF8(doc, "name", name);
    BSON_APPEND_INT32(doc, "id", id);
    BSON_APPEND_INT32(doc, "count", 1);
    BSON_APPEND_DOUBLE(doc, "value", value);

    if (mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
      fprintf(stderr, "%s\n", error.message);
    }

    // 2
    sprintf(temp, "sensor%d", id);
    collection = mongoc_client_get_collection(client, database, temp);

    // 3

    doc = bson_new();
    bson_oid_init(&oid, NULL);
    BSON_APPEND_OID(doc, "_id", &oid);
    BSON_APPEND_INT32(doc, "time", time);
    BSON_APPEND_DOUBLE(doc, "value", value);
    BSON_APPEND_INT32(doc, "idx", 1);

    if (mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
      fprintf(stderr, "%s\n", error.message);
    }
  }
}

int checkIfPipeExists(const char *pipename)
{
  FILE *file;
  if (open("./fifo", O_RDWR, 0) < 0) {
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[])
{
  char astr[MAXBUF];
  char msg[MAXBUF];
  char *buf;

  /* Print Environment Variable */
  printf("HTTP/1.0 200 OK\r\n");
  printf("Server: My Web Server\r\n");

  buf = Getenv("CONTENT_LENGTH");
  int contentLength = atoi(buf);
  printf("Content-Length: %d\r\n", contentLength);

  printf("Content-Type: text/plain\r\n\r\n");

  Rio_readn(STDIN_FILENO, astr, contentLength);

  strcpy(msg, astr);

  printf("%s\n", astr);
  fflush(stdout);

  updateDB(astr);

  int fd;
  fd = Open("./fifo", O_RDWR, 0);
  Write(fd, &contentLength, sizeof(int));
  Write(fd, msg, contentLength);

  int pid = Fork();
  if (pid == 0) {
      Execve("./alarmClient", argv, environ);
  }

  return(0);
}
