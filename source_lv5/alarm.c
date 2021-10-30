#include "stems.h"

int main(int argc, char *argv[])
{
    char astr[MAXBUF];
    char msg[MAXLINE];
    char *buf;
    char name[MAXLINE];
    clock_t time;
    float value;
    char *token;

    /* Print Environment Variable */
    printf("HTTP/1.0 200 OK\r\n");
    printf("Server: My Alarm Server\r\n");

    buf = Getenv("CONTENT_LENGTH");
    int contentLength = atoi(buf);

    Rio_readn(STDIN_FILENO, astr, contentLength);

    /* strtok query */
    token = strtok(astr, "="); // name
    token = strtok(NULL, "&"); // name value
    strcpy(name, token);
    token = strtok(NULL, "="); // time
    token = strtok(NULL, "&"); // time value
    time = atol(token);
    token = strtok(NULL, "="); // value
    token = strtok(NULL, "&"); // value value
    value = atof(token);

    sprintf(msg, "WARNING: name = %s, time = %s, value = %f\n", name, ctime(&time), value);
    Rio_writen(STDERR_FILENO, msg, sizeof(msg));
    fflush(stdout);
}