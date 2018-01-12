#include "sorter.h"
#include "mergesort.c"
#include "directory.c"

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


//Main funcction that contains the multithreading
int main(int argc, char **argv) {
  if((argc < 3)|| (argc > 7)){
  	printf("Invalid number of inputs\n");
  	return;
  }

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
  
  if((argc == (cInt+1)) || (argc == (dInt+1)) || (argc == (oInt+1))){
	printf("PARAMETER ERROR\n");
	return;
  }

  //The search parameter (cPara) is always true... so
  //Here, it'll get the right parameter and get the thisDir and thatDir
  //It goes for every options
  searchForPara = argv[cInt+1];
  char thisDir[512];
  // printf("searchFor:%s\n",searchFor);
  if(cPara == true && dPara == false && oPara == false){ //Works
    // ./sorter -c movie_title
    strcpy(thisDir,"./");
    strcpy(thatDir,"./");
    // printf("1-thisDir:%s\n",thisDir);
  } else if (cPara == true && dPara == true && oPara == false){ //This section is good
    // ./sorter -c movie_title -d thisDir
    strcpy(thisDir, argv[dInt+1]);
    strcpy(thatDir, argv[dInt+1]);
    // printf("2-thisDir:%s\n",thisDir);
  } else if (cPara == true && dPara == false && oPara == true) { //Works as well...
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
  char myDir[512];
  strcpy(myDir,thatDir);
  strcat(myDir,"/stdout"); // ./csvFileDir/stdout

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
  strcat(thatDir,"/");

  while(makeThat != totalNumberOfProcess){
    char word[512] = "";

    int pH = 0;
    int finalLen = strlen(fileSys[makeThat].fileDir);
    int startLen = strlen(thisDir);
    for(pH = 0; pH < finalLen; pH++){
      word[pH] = fileSys[makeThat].fileDir[startLen];
      startLen++;
    }
    word[pH+1] = '\0';

    char temp[512] = "";

    strcpy(temp,thatDir);
    strcat(temp,word);

    if(temp[strlen(temp)-1] != '/'){
      strcat(temp,"/");
    }

    strcpy(fileSys[makeThat].fileThatDirMake,temp);

    makeThat++;
  }

 //This makes the name of the sorted file
  char finalCSVName[256];
  strcpy(finalCSVName, thatDir);
  strcat(finalCSVName, "AllFiles-sorted-");
  // strcpy(finalCSVName, "AllFiles-sorted-");
  strcat(finalCSVName,searchForPara);
  strcat(finalCSVName,".csv");
  finalCSV = fopen(finalCSVName, "w+");

  //this is our temporary csv file where we store all of our csv data
  tempCSV = fopen("tempCSV.csv", "w+");

  FILE *stdout = fopen(myDir,"w+"); //Write into it...

  int newK;
  pthread_t tids[totalNumberOfProcess];
//creates the threads for each file to process
  for(newK = 0; newK < totalNumberOfProcess; newK++) {
    pthread_create(&tids[newK], NULL, mainSorting, &newK);
  }
//joins the threads for every thread created
  for(newK = 0; newK < totalNumberOfProcess; newK++) {
    pthread_join(tids[newK], NULL);
  }

  int x = 0;
  fprintf(tempCSV,"%s\n",firstSent);
  for(newK = 0; newK < totalNumberOfProcess; newK++) {
    for(x = 0; x < fileSentences[newK].amountLine; x++) {
      fprintf(tempCSV,"%s\n",fileSentences[newK].fileLine[x]);
    }
  }

  rewind(tempCSV);

//prints tids for all threads created to stdout 
  fprintf(stdout,"Initial PID: %d\n     TIDS of all child threads: ",pthread_self());
  for(newK = 0; newK < totalNumberOfProcess; newK++) {
    fprintf(stdout,"%d",tids[newK]);
    if(newK < totalNumberOfProcess - 1){
      fprintf(stdout,",");
    }
  }
  fprintf(stdout,"\nTotal Number of Threads: %d\n",totalNumberOfProcess);

  //sort tempcsv && fprintf sorted tempcsv to final csv
  sorttempcsv();
  fclose(tempCSV);
  fclose(finalCSV);
  //delete temp csv
  int ret;
  ret = remove("tempCSV.csv");
  return 0;
}
