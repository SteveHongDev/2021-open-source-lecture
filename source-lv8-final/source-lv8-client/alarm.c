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

    char timeToCTime[MAXLINE];
    strcpy(timeToCTime, ctime(&time));
    if (timeToCTime[strlen(timeToCTime)-1] == '\n')
        timeToCTime[strlen(timeToCTime)-1] = '\0';



    sprintf(msg, "WARNING: name = %s, time = %s, value = %f\r\n\r\n", name, timeToCTime, value);
    Write(STDERR_FILENO, msg, strlen(msg));
    
    fflush(stdout);
}
