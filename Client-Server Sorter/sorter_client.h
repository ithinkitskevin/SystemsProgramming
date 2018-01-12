#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <netdb.h>

#include <fcntl.h>
// #include <sys/sendfile.h>

char *searchForPara;
char thatDir[512];
char *hostName;
int portNum;

FILE * finalCSV;
FILE * tempCSV;

#ifndef SORTERCLIENT_C
#define SORTERCLIENT_C
typedef struct title{
  //Upgraded this to union to save a little bit of data.
  union {
    int intData; //If the data is integer
    //Make the string
    char* stringData; //If the data is character, string or anything
  } data;
  //int row; //This will help me print out the row that the data is in to the new text file
  char *fullData; //This will keep the full sentence data, so that we don't need to go back and get it.
} record;

#endif // SORTER_C
