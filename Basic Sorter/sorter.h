#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#ifndef SORTER_C
#define SORTER_C
typedef struct title{
  int intData; //If the data is integer
  char* stringData; //If the data is character, string or anything
  //int row; //This will help me print out the row that the data is in to the new text file
  char *fullData; //This will keep the full sentence data, so that we don't need to go back and get it.
} record;
#endif // SORTER_C
