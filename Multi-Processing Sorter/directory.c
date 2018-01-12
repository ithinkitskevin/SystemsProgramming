#include "sorter.h"
//This will contain the functions that can open the directories and get the files inside. this will be called list_dir
//The list_dir function will contain another function that can sort out all the .csv files.

//Only problem now is how can we make this into a multiprocess???
bool dealingWithInt;
int nullArrLoc = 0;
int fullArrLoc = 0;

char fileList[255][512]; // The full file list
char fileDir[255][512]; // The directory the file will be at
char fileName[255][512]; //The name of the file will be at
char fileThatDirMake[255][512];

//Counts the process.
static int totalNumberOfProcess = 0;
static int counter = 0;

//A mainSorting function, made into a "single" function so that we can make it to multiProcesses
void mainSorting(char* searchForPara, FILE *fp, int numLine, int atFile, char *thatDir){
  char string[1028] = "";
  char *tempStr = NULL;
  int x = 0;

  char* searchFor = searchForPara;
  int searchInd = 0;

  char *unDataType = NULL;
  int afterComma = 0;

  int preC;

  x = 0;
  //Call in the dirOpen
  record nullArr[numLine];  //This is where the null (a line that doesn't have anything) goes
  record fullArr[numLine];  //This is where a full (a line that has something) goes

  while(fgets(string, sizeof(string), fp) != NULL) {     //read each line of the file
    afterComma = 0;

    // fgets(string, 1028, newFile);
    strtok(string,"\n");

    if(x == 0){
      //We know that if x == 0, then it's the initial start which will mean that we'll get the column's name
      char *tmp = NULL;
      tmp = strdup(string);

      searchInd = searchUp(searchFor, tmp);

      free(tmp);

      x++;
      continue;
    }

    unDataType = personStrtok(string, ',',searchInd);

    // printf("sorter.c:[%s]\n",unDataType);

    //This is where you'll see if the dataTypeis null...
    //If it's null, it'll be placed in a temp struct array
    //And after everything's sorted out, it'll be put back into the main array.
    if(strcmp(unDataType,"") == 0){
      nullArr[nullArrLoc].data.stringData = NULL;
      nullArr[nullArrLoc].fullData = strdup(string);
      nullArrLoc++;
      x++;
      continue;
    }

    //See if it's either string or int here
    char *test = unDataType;
    if(isDigitMine(test)){
      fullArr[fullArrLoc].data.intData = atoi(test);
      dealingWithInt = true;
    } else {
      fullArr[fullArrLoc].data.stringData = strdup(test);
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

  // We have the directory for each, so make the file here...
  // To first make the file, we'll need to make the name of the file as well as putting the directory name initially
  char newDir[512];
  strcpy(newDir,fileThatDirMake[atFile]);

  struct stat st = {0};

  // printf("%s\n",fileThatDirMake[atFile]);

  if (stat(fileThatDirMake[atFile], &st) == -1) {
    // printf("Made directory %s\n",fileThatDirMake[atFile]);
    mkdir(fileThatDirMake[atFile], 0700);
  }

  size_t noLen;
  char *noCSVFile;

  noLen = strlen(fileName[atFile]);

  noCSVFile = strndup(fileName[atFile], noLen >= 4 ? noLen - 4 : 0);

  strcat(newDir,noCSVFile);

  //This is where we put the -sorted- file as well as putting what parameter we're looking for, AS well as putting an end .csv
  strcat(newDir,"-sorted-");
  strcat(newDir,searchForPara);
  strcat(newDir,".csv");

  // printf("newDir:%s\n",newDir);
  //movie_metadata.csv -> movie_metadata-sorted-movie_title.csv

  //So now, using the newDir that we were working on, we'll make the file called fileNew
  FILE *fileNew = fopen(newDir,"w");
  //And now we're going to fprintf the result
  int k = 0;
  if(dealingWithInt){
    // printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      fprintf(fileNew,"%s\n",nullArr[k].fullData);
      free(nullArr[k].fullData);
    }

    // printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      fprintf(fileNew,"%s\n",fullArr[k].fullData);
      free(fullArr[k].fullData);
    }
  } else {
    // printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      fprintf(fileNew,"%s\n",nullArr[k].fullData);
      free(nullArr[k].data.stringData);
      free(nullArr[k].fullData);
    }

    // printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      fprintf(fileNew,"%s\n",fullArr[k].fullData);
      free(fullArr[k].data.stringData);
    }
  }
}


void listDir (char * thisDir) {
  DIR * dir = opendir (thisDir);

  //This will only happen for the first directory call
  if(!dir){
    printf("Can't Open the Directory!\n");
  }

  while (true) {
    struct dirent * insideDir = readdir(dir);

    if (!insideDir) {
      break;
    }

    if (!strcmp(strrchr(insideDir->d_name, '\0') - 4, ".csv")) {
      // Putting everything into a file Array to do later on...
      strcpy(fileDir[totalNumberOfProcess],thisDir);
      strcpy(fileName[totalNumberOfProcess],insideDir->d_name);

      // buffDir is the final directory, and we're first copying the directory name
      char buffDir[512];
      strcpy(buffDir,thisDir);
      int length = strlen(thisDir);

      //Directory Seperate
      buffDir[length] = '/';
      int i = 0;
      while(insideDir->d_name[i] != '\0'){
        buffDir[length + 1 + i] = insideDir->d_name[i];
        i++;
      }
      buffDir[length + 1 + i + 1] = '\0';

      // printf("%s\n",buffDir);

      strcpy(fileList[totalNumberOfProcess], buffDir);
      counter++;
      totalNumberOfProcess++;

      // mainSorting(searchForPara,fp);

      // cleaningUp(nullArr,fullArr,nullArrLoc,fullArrLoc);
    }

    //Recursively go through IF the d_type is a DT_DIR (directory...)
    if (insideDir->d_type == DT_DIR) {
      //The directory will be here
      if (strcmp (insideDir->d_name, "..") != 0 && strcmp (insideDir->d_name, ".") != 0) {
        //Now we have to get the subdirectory, and recursively call that
        char path[PATH_MAX];
        //Keep the pathLength, everything literally breaks without it... Maybe because snprintf returns int????
        int pathLength = snprintf (path, PATH_MAX, "%s/%s", thisDir, insideDir->d_name);
        strcpy(fileThatDirMake[counter],insideDir->d_name);
        listDir (path);
      }
    }

  }
  closedir (dir);
}
