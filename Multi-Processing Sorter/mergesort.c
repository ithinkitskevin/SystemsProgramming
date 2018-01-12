#include "sorter.h"

void mergeForReal(record arr[], int low, int mid, int high, int midLow, int midHigh,bool dealingWithInt) {
  int pL = 0, pR = 0, lowEx = 0;

  int curr = 0;

  record lowTemp[midLow];
  record highTemp[midHigh];

  //Substitute all the low-numbered record from the main
  while(pL < midLow){
    curr = low + pL;
    // printf("we're at pos:%d ",curr);
    if(dealingWithInt){
      // printf(" with data:%d\n",arr[curr].data.intData);
      lowTemp[pL].data.intData = (arr[curr].data.intData);
    } else {
      // printf(" with data:%s\n",arr[curr].data.stringData);
      lowTemp[pL].data.stringData = (arr[curr].data.stringData);
    }

    lowTemp[pL].fullData = (arr[curr].fullData);
    pL++;
  }

  //Substitute all the high-numbered record from the main
  while(pR < midHigh){
    curr = mid + pR + 1;
    // printf("we're at pos:%d ",curr);
    if(dealingWithInt){
      // printf(" with data:%d\n",arr[curr].data.intData);
      highTemp[pR].data.intData = (arr[curr].data.intData);
    } else {
      // printf(" with data:%s\n",arr[curr].data.stringData);
      highTemp[pR].data.stringData = (arr[curr].data.stringData);
    }
    highTemp[pR].fullData = (arr[curr].fullData);

    pR++;
  }


  //Now that we placed stuff in the temporary low or high,
  //We now need put back all the temp stuff back into the
  //main array
  pL = 0;
  pR = 0;
  lowEx = low;
  while (pL < midLow && pR < midHigh) {
    if(dealingWithInt){
      if (lowTemp[pL].data.intData <= highTemp[pR].data.intData){
        // printf("%d is smaller/equal than %d\n",L[i].data.intData,R[j].data.intData);
        arr[lowEx].data.intData = (lowTemp[pL].data.intData);
        arr[lowEx].fullData = (lowTemp[pL].fullData);
        pL++;
      } else {
        // printf("%d is bigger than %d\n",L[i].data.intData,R[j].data.intData);
        arr[lowEx].data.intData = (highTemp[pR].data.intData);
        arr[lowEx].fullData = (highTemp[pR].fullData);
        pR++;
      }
      lowEx++;
    } else {
      if (strcmp(lowTemp[pL].data.stringData,highTemp[pR].data.stringData) == 0 || strcmp(lowTemp[pL].data.stringData,highTemp[pR].data.stringData) < 0) {
        // printf("%s is smaller/equal than %s\n",L[i].data.stringData,R[j].data.stringData);
        arr[lowEx].data.stringData = (lowTemp[pL].data.stringData);
        arr[lowEx].fullData = (lowTemp[pL].fullData);
        pL++;
      } else {
        // printf("%s is bigger than %s\n",L[i].data.stringData,R[j].data.stringData);
        arr[lowEx].data.stringData = (highTemp[pR].data.stringData);
        arr[lowEx].fullData = (highTemp[pR].fullData);
        pR++;
      }
      lowEx++;
    }
  }

  for(; pL < midLow; pL++){
    if(dealingWithInt){
      arr[lowEx].data.intData = (lowTemp[pL].data.intData);
    } else {
      arr[lowEx].data.stringData = (lowTemp[pL].data.stringData);
    }
    arr[lowEx].fullData = (lowTemp[pL].fullData);
    lowEx++;
  }

  for(; pR < midHigh; pR++){
    if(dealingWithInt){
      arr[lowEx].data.intData = (highTemp[pR].data.intData);
    } else {
      arr[lowEx].data.stringData = (highTemp[pR].data.stringData);
    }
    arr[lowEx].fullData = (highTemp[pR].fullData);
    lowEx++;
  }
}

void mergeRec(record recArr[], int low, int high, bool dealingWithInt){
  //This is a merge done recursive.
  //I first used non-recursive method (somewhere down below commented out), but
  //it failed to sort the first/top-middle data for some reason.
  if (low < high) {
    int mid = low+(high-low)/2;

    mergeRec(recArr, low, mid, dealingWithInt);
    mergeRec(recArr, (mid+1), high, dealingWithInt);

    int midLow = mid - low + 1;
    int midHigh = high - mid;

    mergeForReal(recArr, low, mid, high, midLow, midHigh,dealingWithInt);
  }
}

//This takes an array of characters and tells rather or not
//the char in the string is digit or not.
bool isDigitMine(char *input){
  //For negative number
  if (*input == '-'){
    input++;
  }

  //As the char goes by
  while (*input) {
    if (!*input) {
       return false;
   }
   if (!isdigit(*input)){
      return false;
    }
    input++;
  }

  return true;
}

//Trims up the trailing spaces.
char *rtrim(char *s) {
    char* back = NULL;
    int len = strlen(s);

    if(len == 0)
        return(s);

    back = s + len;
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

//Tokenizes each lines of file and then gets the string
//of the line using search index (I talked about that at sorter.c)
char *personStrtok(char *s, char delim, int searchInd){
  int index = 0;

  int arrLoc = 0;
  int charLoc = 0;

  //Tmp array of string
  char arr[64][128];
  char *tmp = NULL;

  bool start = true;
  bool inAStr = false;
  bool hasGoneFurther = false;

  //This is where I go through the string and tokenizes them.
  for(index = 0; index < strlen(s); index++){
    // printf("%d:%c\n",index,s[index]);
    if(arrLoc > searchInd){
      break;
    }

    if(s[index] == ' ' && start == true){
      continue;
    }

    //This and the below if statement will see if we're dealing with
    //A title of a movie that has comma.
    if(s[index] == '"' && inAStr == false){
      inAStr = true;
      continue;
    }

    //Checking inside the movie title ""
    if(s[index] == '"' && inAStr == true){
      //The String Ends here
      hasGoneFurther = true;
      arr[arrLoc][charLoc] = s[index];
      arr[arrLoc][charLoc+1] = '\0';
      start = false;
      charLoc++;
      inAStr = false;
      continue;
    }

    //This is where I seperate the string using delim, which is ,
    if(s[index] == delim && inAStr == false){
      arr[arrLoc][charLoc] = '\0';
      // printf("arr[%d]:%s\n",arrLoc, arr[arrLoc]);
      arrLoc++;
      start = true;
      charLoc = 0;
      continue;
    } else {
      arr[arrLoc][charLoc] = s[index];
      arr[arrLoc][charLoc+1] = '\0';
      start = false;
      // printf("curr at %d:%s\n",arrLoc, arr[arrLoc]);
    }

    charLoc++;
  }

  //Just getting rid of extra spaces. Not really needed, but just in case.
  char* final = rtrim(arr[searchInd]);

  return final;
}
