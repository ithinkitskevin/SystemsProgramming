#include "sorter.h"

bool dealingWithInt;
int nullArrLoc = 0;
int fullArrLoc = 0;

static int threadCount = 0;

char firstSent[1028] = "";

typedef struct file{
  char fileList[2056]; // The full file list
  char fileDir[2056]; // The directory the file will be at
  char fileName[2056]; //The name of the file will be at
  char fileThatDirMake[2056];
  int curr;
} files;

typedef struct allSent{
  char fileLine[8112][1028];
  int amountLine;
} fileSent;

fileSent fileSentences[2056];

files fileSys[2056];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int totalNumberOfProcess = 0;
static int counter = 0;

//Since we're using file systems instead of stdin, I can count them using personally designed
//countSent function. After it counts the file in a directory, it'll rewind the file.
int countSent(FILE *file) {
  int numLines = 0;
  char string[1028] = "";

  while(fgets(string, sizeof(string), file) != NULL){
    numLines++;
  }

  rewind(file);
  return numLines;
}

//this will take the unsorted tempCSV, sort it, and store its output to finalCSV
void sorttempcsv(){
  rewind(tempCSV);
//count the number of lines in tempCSV
  int numLine = countSent(tempCSV);

  char string[1028] = "";
  char *tempStr = NULL;
  int x = 0;

  char* searchFor = searchForPara;
  int searchInd = 0;

  char *unDataType = NULL;
  int afterComma = 0;

  int preC;

  x = 0;

  record nullArr[numLine];  //This is where the null (a line that doesn't have anything) goes
  record fullArr[numLine];  //This is where a full (a line that has something) goes

  fullArrLoc = 0;
  nullArrLoc = 0;

  while(fgets(string, sizeof(string), tempCSV) != NULL) {     //read each line of the file
    afterComma = 0;

    strtok(string,"\n");

    if(x == 0){
      char *tmp = NULL;
      tmp = strdup(string);
      searchInd = searchUp(searchFor, tmp);
      free(tmp);
      x++;
      continue;
    }

    unDataType = personStrtok(string, ',',searchInd);

  //   //This is where you'll see if the dataTypeis null...
  //   //If it's null, it'll be placed in a temp struct array
  //   //And after everything's sorted out, it'll be put back into the main array.
    if(strcmp(unDataType,"") == 0){
      nullArr[nullArrLoc].data.stringData = NULL;
      nullArr[nullArrLoc].fullData = strdup(string);
      nullArrLoc++;
      x++;
      continue;
    }

  //   //See if it's either string or int here
    char *test = unDataType;
    if(isDigitMine(test)){
      fullArr[fullArrLoc].data.intData = atoi(test);
      dealingWithInt = true;
    }else {
      fullArr[fullArrLoc].data.stringData = strdup(test);
     
      dealingWithInt = false;
    }
    fullArr[fullArrLoc].fullData = strdup(string);
    fullArrLoc++;
	x++;
  }

  mergeRec(fullArr, 0, fullArrLoc-1, dealingWithInt);
  int k = 0;
  pthread_mutex_lock(&mutex);

  if(dealingWithInt){
     //printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      fprintf(finalCSV,"%s",nullArr[k].fullData);
      free(nullArr[k].fullData);
    }

     //printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      fprintf(finalCSV,"%s",fullArr[k].fullData);
      free(fullArr[k].fullData);
    }
  } else {
     //printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      fprintf(finalCSV,"%s",nullArr[k].fullData);
      free(nullArr[k].data.stringData);
      free(nullArr[k].fullData);
    }

   //  printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      fprintf(finalCSV,"%s",fullArr[k].fullData);
      free(fullArr[k].data.stringData);
    }
  }
  pthread_mutex_unlock(&mutex);

};

void *mainSorting(void *arg) {
//(char* searchForPara, FILE *fp, int numLine, int atFile, char *thatDir){
  int current = *((int *)arg);
  //Doesn't work for non-Linux machines
  // tids[current] = syscall(__NR_gettid);

  FILE *fp = fopen(fileSys[current].fileList,"r");
  int line = countSent(fp);

  fileSentences[current].amountLine = line-1;

  char string[1028] = "";

  //critical section
  //CAUTION: don't remove the lock otherwise when dealing with more than 1 csv file, a segfault will occur.
  pthread_mutex_lock(&mutex);

  int x = 0;

  threadCount = 0;
  while(fgets(string, sizeof(string), fp) != NULL) {     //read each line of the file
    if(threadCount == 0){
      strcpy(firstSent,string);
      threadCount++;
      continue;
    }
    strcpy(fileSentences[current].fileLine[x],string);
    x++;
  }

  pthread_mutex_unlock(&mutex);
  //critical section end

  pthread_exit(0);

}//end of mainsorting


void listDir (char * thisDir) {
  DIR * dir = opendir (thisDir);

  if(!dir){
    printf("Can't Open the Directory!\n");
  }

  while (true) {
    struct dirent * insideDir = readdir(dir);

    if (!insideDir) {
      break;
    }

    if (!strcmp(strrchr(insideDir->d_name, '\0') - 4, ".csv")) {
      strcpy(fileSys[totalNumberOfProcess].fileDir,thisDir);
      strcpy(fileSys[totalNumberOfProcess].fileName,insideDir->d_name);

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

      strcpy(fileSys[totalNumberOfProcess].fileList, buffDir);
      counter++;
      totalNumberOfProcess++;

    }

    if (insideDir->d_type == DT_DIR) {
      if (strcmp (insideDir->d_name, "..") != 0 && strcmp (insideDir->d_name, ".") != 0) {
        char path[PATH_MAX];
        int pathLength = snprintf (path, PATH_MAX, "%s/%s", thisDir, insideDir->d_name);
        strcpy(fileSys[counter].fileThatDirMake,insideDir->d_name);
        listDir (path);
      }
    }

  }
  closedir (dir);
};

/*
for(i = 0; i < n; i++)
    {
        rc = pthread_create(&thread_id[i], NULL, PrintHello, (void*)i);
        if(rc)
        {
             printf("\n ERROR: return code from pthread_create is %d \n", rc);
             exit(1);
        }
        printf("\n I am thread %d. Created new thread (%d) in iteration %d ...\n",
                pthread_self(), thread_id[i], i);
        if(i % 5 == 0) sleep(1);
    }

*/
