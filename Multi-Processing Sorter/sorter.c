#include "sorter.h"
#include "mergesort.c"
#include "directory.c"

/*
Few things of this program...

This program doesn't account for
./sorter -c food -d thisdir/thatdir
The way the parameters are set up in this command, in my opinion, is not a good idea (it was also optional).
Using a forward slash as a way to differentiate thisdir and thatdir is strange, as a directory already contains a forward slash

If the column we're looking for is not found, then I've just printed out everything else non-sorted

the parameter, thatDir, needs to already be existing for this program to work. But if the folders within folders do not exist,
the program will make that folder.

This program will, instead of traversing directories and processing each .csv file, will first traverse the directories and
will remember each directories as well as the names of the .csv files only.
After traversing the directories, it'll then fork over each .csv file, so the number of .csv files there are is the number of processes
this program will use. And after they mergesort and such, it'll fprintf the nullArr first then fullArr.
*/

//Since we're using file systems instead of stdin, I can count them using personally designed
//countSent function. After it counts the file in a directory, it'll rewind the file.
//Btw, the previous code (for Project 1) made 65535 of them fullArr and nullArr, wasting SO much memory...
//So instead of wasting memory, I'm going to waste a little bit of processes by going though the whole file and counting up the lines.
int countSent(FILE *file) {
  int numLines = 0;
  char string[1028] = "";

  while(fgets(string, sizeof(string), file) != NULL){
    numLines++;
  }

  rewind(file);
  return numLines;
}

// This function will get the #word of the parameter. Then using this, we can go through line by line and get the word.
// This function will only run once, and in the first line only
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

//Main funcction that contains the multiprocess
int main(int argc, char **argv) {
  //Searching for the right parameter, and just in case, I'll go through a while loop so that wherever the parameter is
  //we'll be able to get the thisDir, thatDir, and search word
  int paraInt;
  char *thisChar;

  bool cPara = false;
  int cInt = 0;
  bool dPara = false;
  int dInt = 0;
  bool oPara = false;
  int oInt = 0;

  for (paraInt = 1; paraInt < argc; paraInt++) {
    thisChar = argv[paraInt];
    if(strcmp(thisChar,"-c") == 0){
      cPara = true;
      cInt = paraInt;
    } else if (strcmp(thisChar,"-d") == 0) {
      dPara = true;
      dInt = paraInt;
    } else if (strcmp(thisChar,"-o") == 0){
      oPara = true;
      oInt = paraInt;
    }
  }

  //The search parameter (cPara) is always true... so
  //Here, it'll get the right parameter and get the thisDir and thatDir
  //It goes for every options
  searchForPara = argv[cInt+1];
  char thisDir[512];
  char thatDir[512];
  // printf("searchFor:%s\n",searchFor);
  if(cPara == true && dPara == false && oPara == false){
    // ./sorter -c movie_title
    strcpy(thisDir,"./");
    strcpy(thatDir,"./");
    // printf("1-thisDir:%s\n",thisDir);
  } else if (cPara == true && dPara == true && oPara == false){ //This section is good
    // ./sorter -c movie_title -d thisDir
    strcpy(thisDir, argv[dInt+1]);
    strcpy(thatDir, argv[dInt+1]);
    // printf("2-thisDir:%s\n",thisDir);
  } else if (cPara == true && dPara == false && oPara == false){
    // ./sorter -c movie_title -o thatDir
    strcpy(thisDir,"./");
    strcpy(thatDir,argv[oInt+1]);
    // printf("3-thatDir:%s\n",thatDir);
  } else if (cPara == true && dPara == true && oPara == true){ //This section is also good...
    // ./sorter -c movie_title -d thisDir -o thatDir
    strcpy(thisDir, argv[dInt+1]);
    strcpy(thatDir, argv[oInt+1]);
    // printf("4-thatDir:%s\n",thatDir);
    // printf("4-thisDir:%s\n",thisDir);
  }

  DIR* checkThatDir = opendir(thatDir);
  if (checkThatDir) {
    closedir(checkThatDir);
  } else if (ENOENT == errno) {
    printf("DIRECTORY NOT FOUND (that)\n");
    return 0;
  } else {
    printf("DIRECTORY ERROR (that)\n");
    return 0;
  }

  DIR* checkThisDir = opendir(thisDir);
  if (checkThisDir) {
    closedir(checkThisDir);
  } else if (ENOENT == errno) {
    printf("DIRECTORY NOT FOUND (this)\n");
    return 0;
  } else {
    printf("DIRECTORY ERROR (this)\n");
    return 0;
  }

  //This function will go thorugh the entire directory, using the thisDir parameter...
  listDir(thisDir);
  //Now we have file Directory, the full file with its directory, and the file's name.

  //This is where multiprocessing happens
  int k = 0;

  //Creating stdout and the directory for it
  char myDir[128];
  strcpy(myDir,thatDir);
  strcat(myDir,"/stdout"); // ./csvFileDir/stdout

  pid_t childPID;
  pid_t parentPID;
  pid_t initalPID;
  int stat;

  //By the way, we're going to READ the files from fileList
  //BUT we're going to MAKE the files using fileDir and fileName
  //The new file array called fileThatDirMake will be an array that will MAKE the file using information from fileDir and fileName
  int makeThat = 0;

  char substr[512] = "";

  if(thatDir[strlen(thatDir)-1] == '/'){
    int tempLen = strlen(thatDir)-1;
    strncpy(substr, thatDir, tempLen);
    strcpy(thatDir,substr);
  }
  strcat(thatDir,"/");//Yeah, I know... I could have done this alot better...

  while(makeThat != totalNumberOfProcess){
    char word[512] = "";

    int pH = 0;
    int finalLen = strlen(fileDir[makeThat]);
    int startLen = strlen(thisDir);
    for(pH = 0; pH < finalLen; pH++){
      word[pH] = fileDir[makeThat][startLen];
      startLen++;
    }
    word[pH+1] = '\0';

    char temp[512] = "";

    strcpy(temp,thatDir);
    strcat(temp,word);

    if(temp[strlen(temp)-1] != '/'){
      strcat(temp,"/");
    }

    strcpy(fileThatDirMake[makeThat],temp);

    makeThat++;
  }

  // int test = 0;
  // while(test != totalNumberOfProcess){
  //   printf("fileList %d:%s\n",test,fileList[test]);
  //   printf("fileDir %d:%s\n",test,fileDir[test]);
  //   printf("fileName %d:%s\n",test,fileName[test]);
  //   // printf("fileThatDirMake %d:%s\n",test,fileThatDirMake[test]);
  //   test++;
  //   printf("\n");
  // }

  FILE *stdout = fopen(myDir,"w"); //Write into it...
  for(k = 0; k < totalNumberOfProcess; k++) {
    childPID = fork();
    initalPID = getppid();

    if(childPID == (pid_t)0){
      childPID = getpid();
      parentPID = getppid();

      if(k == 0){
        fprintf(stdout,"Inital PID: %d\n",parentPID);
        fprintf(stdout,"PIDS of all child processes: ");
      }

      fprintf(stdout,"%d",childPID);
      int temp = totalNumberOfProcess-1;

      if(k != temp){
        fprintf(stdout,", ");
      } else {
        fprintf(stdout,"\n");
        fprintf(stdout,"Total number of processes: %d\n",totalNumberOfProcess);
      }

      FILE *fp = fopen(fileList[k],"r");
      int numLine = countSent(fp);
      mainSorting(searchForPara, fp, numLine, k, thatDir);
      // printf("\n------------------------------------------------\n");
      break;
    } else {
      //Parent Process will just wait to prevent orphans...
      wait(&stat);
    }
  }

  return 0;
}
