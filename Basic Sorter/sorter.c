#include "sorter.h"
#include "mergesort.c"

/*
Document your design, assumptions, difficulties you had and testing procedure. Include any test CSV files you used in your documentation. Be sure to also include a short description of how to use your code. Look at the man pages for suggestions on format and content. Do not neglect your header file. Be sure to describe the contents of it and why you needed them.


So the design of the code is to first get the CSV file. The first line of the CSV file is where the
type of data it is, so at the first Line, I searched up the data we wanted for by getting how many
commas there is before the targeted word was. This is the serachInd. Using that integer, I could
now search the targeted word in my personStrtok function. I had to make my own personStrtok due to
two parameters. First was to tell rather or not we were dealing with "" stuff and the second parameter
was that the empty, NULL data that could be founded in the lines of CSV file will
mess up the whole searching up word algorithm. Thus, I had an if function that saw if there is a NULL. If
so, then I would put it in my nullArr. Otherwise, if the data was fine and dandy, then I would put it in
my fullArr. fullArr and nullArr are struct array that contained intData, strData, and fullData. intData
is data that is integer, strData is data that's a string, and fullData is the line at the currFile line.
This way, I could differentiate them when I was merging, making my life easier. At the end, after merging
up with your usual basic algorithm (except using struct array instead of just array), I printed them out.
First, I printed out nullArr's fullData then the fullArr's fullData. That's it yo.

So to actually use my code, it's pretty simple. It's literally first cat a CSV file you want, then pipe
them into the compiled code. The code needs to thing. First is the first parameter, which is -c. If it's
not there, then I would yell at you. Second is what the data that you want to sort is. If you don't give
that to me, then I just simpliy give everything back. So below is a few example.

First, compile
gcc sorter.c -o sorter

cat input.file | ./sorter -c movie_title
This will print in your terminal the sorted file. But if you want the information in a new file

cat input.file | ./sorted -c movie_title > sortedFile.csv
That way, you can now just vi or nano or whatever into the sortedFile.csv (you can even cat it), and read
the information.
*/

//Kevin Lee//

/********************************************************************************************************/


/*
A public variable that determines rather or not we're
working with strings or integers
*/
bool dealingWithInt;

/*
A once important function that determined how many lines the file had.
Not unused and forgotten since I found out that we can't rewind stdin,
making this method unimportant.
*/
// int countLines(){
//   FILE *fp;
//   fp = stdin;
//   // fp = fopen("movie_metadata.csv", "r");
//
//   if(fp == NULL){
//     printf("Error reading file\n");
//     return 1;
//   }
//
//   int lines = 0;
//   int preC;
//
//   while ((preC = fgetc(fp)) != EOF){
//
//     if (preC == '\n'){
//       lines++;
//     }
/   }
//
//   close(fp);
//   return lines;
// }


/*
A function that searches up the word we're looking for then
gets the searchInd. searchInd is search index, and tells the int
of how many commas it passed. It's important for later.
*/
int searchUp(char *searchFor, char *fullData){
  // printf("where is %s at %s\n",searchFor, fullData);

  int searchInd = 0;
  char *tokenStr = "";

  tokenStr = strtok(fullData,",");
  while(tokenStr != NULL){
    if(strcmp(tokenStr,searchFor) == 0){
      return searchInd;
    }
    tokenStr = strtok(NULL,",");
    searchInd++;
  }

  return searchInd;
}



int main(int argc, char **argv) {
	if (argc != 3){
	  printf("Invalid input. Input must consist of two arguments\n");
	  return 1;
	}

	char *firstArg = argv[1];
  //Exactly what it says, determines if the argument is -c or not.
  if(strcmp(firstArg,"-c") == 1){
    printf("Invalid parameter, exiting");
    return 1;
  }

  // int newLine = 0;

  char string[1028] = "";
  char *tempStr = NULL;
  int x = 0;

  FILE *newFile;
  newFile = stdin;
  // newFile = fopen("movie_metadata.csv", "r");

  char* searchFor = argv[2];
  int searchInd = 0;

  char *unDataType = NULL;
  int afterComma = 0;

  record nullArr[65536];
  record fullArr[65536];

  int nullArrLoc = 0;
  int fullArrLoc = 0;
  int preC;

  x = 0;

  while(fgets(string, sizeof(string), stdin) != NULL) {     //read each line of the file
    afterComma = 0;

    // fgets(string, 1028, newFile);
    strtok(string,"\n");

    // printf("%s\n",string);

    if(x == 0){
      //We know that if x == 0, then it's the initial start which will mean that we'll get the column's name
      char *tmp = NULL;
      tmp = strdup(string);
      searchInd = searchUp(searchFor, tmp);
      //  printf("the x:%d and we're at index:%d\n",x,searchInd);
      free(tmp);
      x++;
      continue;
    }
    // char *temp = personStrtok(string, ',', searchInd);
    // printf("temp:%s\n",temp);


    //Better way to toknize... It'll be faster and be more efficine than the shit on top
    unDataType = personStrtok(string, ',',searchInd);

    // printf("sorter.c:[%s]\n",unDataType);

    //This is where you'll see if the dataTypeis null...
    //If it's null, it'll be placed in a temp struct array
    //And after everything's sorted out, it'll be put back into the main array.
    if(strcmp(unDataType,"") == 0){
      nullArr[nullArrLoc].stringData = NULL;
      nullArr[nullArrLoc].intData = 0;
      nullArr[nullArrLoc].fullData = strdup(string);
      nullArrLoc++;
      x++;
      continue;
    }

    //See if it's either string or int here
    char *test = unDataType;
    if(isDigitMine(test)){
      fullArr[fullArrLoc].intData = atoi(test);
      fullArr[fullArrLoc].stringData = NULL;
      dealingWithInt = true;
    } else {
      fullArr[fullArrLoc].stringData = strdup(test);
      fullArr[fullArrLoc].intData = 0;
      // printf("insideForLoop:%s\n",fullArr[fullArrLoc].stringData );
      dealingWithInt = false;
    }
    fullArr[fullArrLoc].fullData = strdup(string);
    // printf("fullData:%s\n",string);
    fullArrLoc++;

    x++;
  }

  // newLine = x;
  // printf("newLine:%d\n",newLine); //Should be 5044
  // printf("fullArrLoc:%d\n",fullArrLoc); //Should be 5043
  //This is where all the merging happens...
  mergeRec(fullArr, 0, fullArrLoc-1, dealingWithInt);

  //This is where all the printing happens. if(dealingWithInt) was important
  //when dealing with if the data was right, but now we really don't need it
  int k = 0;
  if(dealingWithInt){
    // printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      // printf("int-nullArr:[%d]\n",nullArr[k].intData);
      printf("%s\n",nullArr[k].fullData);
    }

    // printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      // printf("int-fullArrLoc:[%d]\n",fullArr[k].intData);
      printf("%s\n",fullArr[k].fullData);
    }
  } else {
    // printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      // printf("str-nullArr:[%s]\n",nullArr[k].stringData);
      printf("%s\n",nullArr[k].fullData);
    }

    // printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      // printf("str-fullArrLoc:[%s]\n",fullArr[k].stringData);
      printf("%s\n",fullArr[k].fullData);
    }
  }

  cleaningUp(nullArr,fullArr,nullArrLoc,fullArrLoc);
  close(newFile);

  return 0;
}
