#include "sorter_server.h"
#include "mergesort.c"

#define FILENAME "tmpCSV.csv"

typedef struct choose{
  int sd;
  int loc;
  bool occu;
} ipAddAndX;

ipAddAndX at[MAX_CLIENT-1];

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

void initalize(){
  int x = 0;

  for(x = 0; x < MAX_CLIENT; x++){
    at[x].sd = 0;
    at[x].loc = -1;
    at[x].occu = false;
  }
}

int countSent(FILE *file) {
  int numLines = 0;
  char string[1028] = "";

  while(fgets(string, sizeof(string), file) != NULL){
    numLines++;
  }

  rewind(file);
  return numLines;
}

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sorttempcsv(){
  bool dealingWithInt;
  int nullArrLoc,fullArrLoc;

//  rewind(tempCSV);
//count the number of lines in tempCSV
  int numLine = countSent(tempCSV);
//  printf("numLine - %d\n",numLine);

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

/* int te = 0;
  while(te < fullArrLoc){
    printf("%d at %s\n",te, fullArr[te].data.stringData);
    ++te;
  }
*/
  mergeRec(fullArr, 0, fullArrLoc-1, dealingWithInt);
  int k = 0;
  // pthread_mutex_lock(&mutex);

  // finalCSV = fopen("sorterServer.csv","w+");
  finalCSV = fopen("new.csv","w+");
  fflush(finalCSV);
  rewind(finalCSV);

  if(dealingWithInt){
     //printf("nullArrLoc:%d\n",nullArrLoc);
    for( k = 0; k < nullArrLoc; k++){
      // printf("%s\n",nullArr[k].fullData);
      fprintf(finalCSV,"%s\n",nullArr[k].fullData);
      free(nullArr[k].fullData);
    }

     //printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      // printf("%s\n",fullArr[k].fullData);
      fprintf(finalCSV,"%s\n",fullArr[k].fullData);
      free(fullArr[k].fullData);
    }
  } else {
     //printf("nullArrLoc:%d\n",nullArrLoc);

    for( k = 0; k < nullArrLoc; k++){
      // printf("%s\n",nullArr[k].fullData);
      fprintf(finalCSV,"%s\n",nullArr[k].fullData);
      free(nullArr[k].data.stringData);
      free(nullArr[k].fullData);
    }

   //  printf("fullArrLoc:%d\n",fullArrLoc);
    for( k = 0; k < fullArrLoc; k++){
      // printf("%s\n",fullArr[k].fullData);
      fprintf(finalCSV,"%s\n",fullArr[k].fullData);
      free(fullArr[k].data.stringData);
    }
  }
  // pthread_mutex_unlock(&mutex);
  fclose(finalCSV);
};

char *perItoa(int num, char *str) {
  if(str == NULL){
    return NULL;
  }
  sprintf(str, "%d", num);

  return str;
  // char buffer[10];
  // perItoa(sock,buffer);
  // strcat(message, buffer);
}

char *trimwhitespace(char *str) {
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

void *connection_handler(void *argument) {
//mutex lock starts here

//  printf("\n");
  struct stat file_stat;

  ipAddAndX *arg = argument;
  //This is where we should do the organizing
  int sock = arg -> sd;
  // int sock = *(int*)socket_desc;
//  printf("Socket Descriptor is %d\n",sock);

  char message[256], msg[256];

  //get parameter to sort by
  bzero(message, 256);
	strcat(message, "Send parameter to sort by");
	write(sock, message, strlen(message));

	bzero(msg, 256);
	read(sock, msg, 255);
//	printf("sort by: <%s>\n", msg);
	searchForPara = msg;
	// strcpy(searchForPara, msg);

  char *newSearch = malloc(strlen(searchForPara) * sizeof(char));
  strcpy(newSearch,searchForPara);

  //check if sort or dump request
  bzero(message,256);
//  strcat(message, "Press 0 for sort request or Press 1 for dump request");
strcat(message, "Press 0 for sort request");
  write(sock , message , strlen(message));

  int response;
  read(sock, message, strlen(message));
  response = atoi(message);

//  printf("The response is : %d\n",response);


  //Do the sorting part here
  char longMessage[2048];
  char longMessServ[2048];

  char *data;
  bool succ = false;
  FILE *cliCSV;
  if(response == 0){
    while(succ == false){
      cliCSV = fopen("tmpcsv.csv", "w");
      bzero(longMessage, 2048);
			int bp = 0;
      int wp;
			while((bp = recv(sock, longMessage, 2048, 0)) > 0) {
			    wp = fwrite(longMessage, sizeof(char), bp, cliCSV);
				  if(wp < bp){
			        error("File write failed on server.\n");
			    }
				bzero(longMessage, 2048);
				if (bp == 0 || bp != 2048) {
					break;
				}
			}
//      printf("Ok received from client!\n");
      rewind(cliCSV);
			fclose(cliCSV);

      // FILE *test;
      // test = fopen("tmpcsv.csv","r");
      // char string[2048];
      // while(fgets(string, sizeof(string), test) != NULL){
      //   printf("%s",string);
      // }

      tempCSV = fopen("tmpcsv.csv","r");

      sorttempcsv();
     /* FILE *test;
      test = fopen("new.csv","r");
      char string[2048];
      while(fgets(string, sizeof(string), test) != NULL){
        printf("%s",string);
      }*/

//		    printf("[Server] Sending to the Client...");
		    FILE *fs = fopen("new.csv", "r");
		    if(fs == NULL){
		      printf("ERROR\n");
				  exit(1);
		    }

		    bzero(longMessServ, 2048);
		    int bps;
		    while((bps = fread(longMessServ, sizeof(char), 2048, fs))>0) {
		        if(send(sock, longMessServ, bps, 0) < 0) {
		            printf("ERROR\n");
		            exit(1);
		        }
            bzero(longMessServ, 2048);
		    }
		//    printf("Ok sent to client!\n");
		    succ = true;
    }
  } else {
    printf("ERROR on RESPONSE\n");
  }


  arg -> sd = 0;
  arg -> loc = -1;
  arg -> occu = false;

//  printf("should have erased and fin\n");
//fclose(tmpcsv);
  //end mutex lock
  
  int ret;
  ret = remove("tmpcsv.csv");
  ret = remove("new.csv");
}

int main(int argc, char *argv[]) {
  int sockNum;

  if(argc == 3){
    if(strcmp(argv[1],"-p") == 0){
      sockNum = atoi(argv[2]);
    } else {
      printf("ERROR: parameter incorrect, exiting\n");
    }
  } else {
    printf("ERROR: validating socket number\n");
    return 0;
  }

  initalize();

  int paraInt;
  int serverSocket;

  struct sockaddr_in server;
  struct sockaddr_in client;

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket < 0) {
    printf("Error in Creating Socket\n");
  }
//  printf("1. creating server socket done\n");

  bzero((char *) &server, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(sockNum);

  if(bind(serverSocket,(struct sockaddr*) &server, sizeof(server)) < 0){
    printf("ERROR in binding");
  }

//  printf("2. binding server socket done at socket %d\n", sockNum);

  listen(serverSocket, MAX_CLIENT);
//  printf("3. Server is waiting for clients\n");

  int cSize = sizeof(struct sockaddr_in);
  int csd,c;

  int fd;
  struct ifreq ifr;

  char IPAddress[256];

  int pd;

  int n = 0;
  int threadCount = 0;
  printf("Received connections from: ");

    while(1) {
      int clientLen = sizeof(client);
      csd = accept(serverSocket, (struct sockaddr *)&client, &clientLen);
      if(csd < 0){
        printf("Error on Accepting\n");
      }
//      printf("accepting...\n");

      char buffer[256];

      bzero(buffer,256);
      n = read(csd,buffer,256);
      printf("<%s>,", inet_ntoa(client.sin_addr));
//      printf("Hostname from Client to Server: %s\n", buffer); //The buffer should be the host name...
      //These commented lines should replace the ones above

      pthread_t tid;

      int pd = poolCheck(csd);

      while(pd == -1){
        printf("Thread Pool full, please wait");
        sleep(3);
        pd = poolCheck(csd);
      }

      if( pthread_create(&tid, NULL, connection_handler, &at[pd]) < 0) {
        perror("could not create thread");
        return 1;
      }

    }

  return 0;
}
int poolCheck(int csd){
  int pd = 0;
  for(pd = 0; pd < MAX_CLIENT; pd++) {
    if(at[pd].occu == false) {
      at[pd].sd = csd;
      at[pd].loc = pd;
      at[pd].occu = true;

//      printf("New thread %d and csd is %d\n", at[pd].loc, at[pd].sd);
      return pd;
    }
  }
  return -1;
}



/*


    int snum_read;
    int x;
    //open a file to take in the data sent from the client

      // bzero(longMessage, 2048);
      // printf("%d\n",x);
      //OR OR OR
      //We can do the read here while(read>0) or some shit
      //And than we can do the sort Here
      int num;
      while(1) {
        if ((num = recv(sock, longMessage, 1024,0))== -1) {
          printf("Failure Sending Message\n");
          exit(1);
        } else if (num == 0) {
          printf("Connection closed\n");
           //So I can now wait for another client
          break;
        }
        longMessage[num] = '\0';
        printf("Server:Msg Received %s\n", longMessage);
        if ((send(sock,longMessage, strlen(longMessage),0))== -1) {
          printf("Failure Sending Message\n");
          close(sock);
          break;
        }
        printf("Server:Msg being sent: %s\nNumber of bytes sent: %d\n", longMessage, strlen(longMessage));
      }
      printf("FINISHED SENDING\n");
    rewind(tempCSV);

    // FILE *finalSend;
    // finalSend = fopen("new.csv","r");
    //
    // char string[2048] = "";
    // while(fgets(string, sizeof(string), finalSend) != NULL){
    //   write(sock,string,sizeof(string));
    // }
    //end of sort request
  } else if (response != 1){
    printf("Unknown Request\n");
  }

  //dump request starts here
  printf("here\n");
  sorttempcsv();



  FILE *sToC;
  sToC = fopen("new.csv","w+");

  int num = 0;
  while(fgets(longMessage, sizeof(longMessage), sToC) != NULL){
    bzero(longMessage, 2048);

    if((send(sock,longMessage,strlen(longMessage),0)) == -1){
      fprintf(stderr, "Failure Sending Message\n");
      close(sock);
      exit(1);
    } else {
      printf("FINAL[%s]\n",longMessage);
       num = recv(sock, longMessage, sizeof(longMessage),0);
       if ( num <= 0 ) {
        printf("Either Connection Closed or Error\n");
         //Break from the While
         break;
       }
       longMessage[num] = '\0';
       printf("FINAL[%s]\n",longMessage);
    }

  }

*/
