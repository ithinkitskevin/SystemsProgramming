#include "sorter_client.h"

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


void *mainSorting(void *arg) {
  int current = *((int *)arg);

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
      // printf("At %d, the file is %s\n", totalNumberOfProcess,fileSys[totalNumberOfProcess].fileDir);
      strcpy(fileSys[totalNumberOfProcess].fileName,insideDir->d_name);
      // printf("At %d, the file is %s\n", totalNumberOfProcess,fileSys[totalNumberOfProcess].fileName);

      char buffDir[512] = "2";
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
