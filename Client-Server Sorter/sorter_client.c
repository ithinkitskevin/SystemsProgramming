#include "sorter_client.h"
#include "directory.c"

int main(int argc, char **argv) {
   if((argc < 7)|| (argc > 11)){
   	printf("Invalid number of inputs\n");
   	return;
   }

  //we'll be able to get the thisDir, thatDir, search word, server name, port number
  int paraInt;
  char *thisChar;

  bool cPara = false;
  int cInt = 0;
  bool hPara = false;
  int hInt = 0;
  bool pPara = false;
  int pInt = 0;
  bool dPara = false;
  int dInt = 0;
  bool oPara = false;
  int oInt = 0;

  for (paraInt = 1; paraInt < argc; paraInt++) {
    thisChar = argv[paraInt];
    if(strcmp(thisChar,"-c") == 0){
      cPara = true;
      cInt = paraInt;
    } else if (strcmp(thisChar, "-h") == 0){
 	  hPara = true;
      hInt = paraInt;
    } else if (strcmp(thisChar, "-p") == 0){
      pPara = true;
      pInt = paraInt;
    } else if (strcmp(thisChar,"-d") == 0) {
      dPara = true;
      dInt = paraInt;
    } else if (strcmp(thisChar,"-o") == 0){
      oPara = true;
      oInt = paraInt;
    }
  }

  if((argc == (cInt+1)) || (argc == (dInt+1)) || (argc == (oInt+1)) || (argc == (hInt+1)) || (argc == (pInt+1))){
	printf("PARAMETER ERROR\n");
	return;
  }

  //The search parameter (cPara, hPara, pPara) must ALWAYS be  true
   if((cPara == false) || (hPara == false) || (pPara == false)){
   	printf("PARAMETER ERROR: MISSING FLAG\n");
   	return;
   }

  //Here, it'll get the right parameter and get the thisDir and thatDir
  //It goes for every options
  searchForPara = argv[cInt+1];
  hostName = argv[hInt+1];
  portNum = atoi(argv[pInt+1]);
  char thisDir[512];

  if((cPara == true) && (hPara == true) && (pPara == true) && (dPara == false) && (oPara == false)){ //Works
    // ./sorter -c movie_title -h grep.cs.rutgers.edu -p 12345
    strcpy(thisDir,"./");
    strcpy(thatDir,"./");
    // printf("1-thisDir:%s\n",thisDir);
  } else if ((cPara == true) && (hPara == true) && (pPara == true) && (dPara == true) && (oPara == false)){ //This section is good
    // ./sorter -c movie_title -h grep.cs.rutgers.edu -p 12345 -d thisDir
    strcpy(thisDir, argv[dInt+1]);
    strcpy(thatDir, argv[dInt+1]);
    // printf("2-thisDir:%s\n",thisDir);
  } else if ((cPara == true) && (hPara == true) && (pPara == true) && (dPara == false) && (oPara == true)) { //Works as well...
    // ./sorter -c movie_title  -h grep.cs.rutgers.edu -p 12345-o thatDir
    strcpy(thisDir,"./");
    strcpy(thatDir,argv[oInt+1]);
    // printf("3-thatDir:%s\n",thatDir);
  } else if (cPara == true && (dPara == true) && (oPara == true)){ //This section is also good...
    // ./sorter -c movie_title  -h grep.cs.rutgers.edu -p 12345 -d thisDir -o thatDir
    strcpy(thisDir, argv[dInt+1]);
    strcpy(thatDir, argv[oInt+1]);
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

   listDir(thisDir);

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

  // FILE *tmp;
  // tempCSV = fopen("tempCSV.csv", "w+");


 /* printf("column_name: %s\n hostName: %s\n portNum: %d\n", searchForPara, hostName, portNum);*/
  struct hostent *server;
  int sockfd, n, s;
  struct sockaddr_in serv_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0){
  	printf("ERROR opening socket\n");
  	return 0;
  }


 // hostNameToIp(server);
  server = gethostbyname(hostName);
  if(server == NULL){
  	printf("Error, no such host\n");
  	return 0;
  }

  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;

  bcopy((char *)server ->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);

  serv_addr.sin_port = htons(portNum);

  //open the connection to the server
  int connection_status = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if(connection_status < 0){
   	printf("ERROR connecting\n");
  }

  char server_response[256];
  bzero(server_response, 256);
  strcpy(server_response,"hello from client");
  //write to the server
  // strcpy(server_response, hostName);
  n = write(sockfd, server_response, strlen(server_response));
  if(n < 0) {
  	printf("ERROR writing to socket");
  }

  int newK;

//send parameter
  bzero(server_response, 256);
  n = read(sockfd, &server_response, 255);
  if(n < 0){
  	printf("ERROR reading from socket\n");
  }
//  printf("%s\n", server_response);

  write(sockfd, searchForPara, strlen(searchForPara));
//  printf("searchForPara: %s\n", searchForPara);


  //read from the server on what to do next
  bzero(server_response, 256);
  n = read(sockfd, &server_response, 255);
  if(n < 0){
  	printf("ERROR reading from socket\n");
  }
  printf("%s\n", server_response);



  FILE *tmp ;
  FILE * fp ;
//  printf("after file declarations\n");
	char str1[20];
	scanf("%s", str1);
	int response = atoi(str1);
//	printf("response is: %d\n", response);

	n = write(sockfd, &str1, sizeof(str1));
    if(n < 0){
    	printf("ERROR reading from socket\n");
    }

  /*  bzero(server_response, 256);
    read(0,server_response,10);
    n = write(sockfd, &server_response, 255);
    if(n < 0){
    	printf("ERROR reading from socket\n");
    }
    int response = atoi(server_response);
*/

  if(response == 0) {
 //   printf("Sort request\n");

    char string[2048] = "";
    makeThat = 0;
    tmp = fopen("client.csv","w+");
    while(makeThat != totalNumberOfProcess) {
//      printf("File List - %s\n",fileSys[makeThat].fileList);
      string[0] = '\0';
      int x = 0;

      FILE * fp = fopen(fileSys[makeThat].fileList, "r");

      if(fp == NULL){
        makeThat++;
      	continue;
      }
      while(fgets(string, sizeof(string), fp) != NULL){
        if (makeThat == 0){
          fprintf(tmp,"%s",string);
          continue;
        }
        if(makeThat != 0 && x != 0){
          fprintf(tmp,"%s",string);
        }
        x++;
      }
      x = 0;
      makeThat++;
    }
  } else {
    printf("WRONG ERROR TYPE");
  }
  fclose(tmp);



  fp = fopen("client.csv","r");
  char data[2048];
  int readD;
	while((readD = fread(data, sizeof(char), 2048, fp)) > 0) {
	   if(send(sockfd, data, readD, 0) < 0) {
		  printf("ERROR: Failed to send file\n");
		  break;
	   }
    bzero(data, 2048);
	}
//	printf("SENT FILE\n");

	/* Receive File from Server */
	char finalCSVName[256];
      strcpy(finalCSVName, "AllFiles-sorted-");
  		strcat(finalCSVName,searchForPara);
  		strcat(finalCSVName,".csv");
	
  FILE *fr = fopen(finalCSVName, "w");
  char dataServ[2048];

	if(fr == NULL){
		printf("Unable to open File\n");
	} else {
		bzero(dataServ, 2048);
		int sbp = 0;
	   while((sbp = recv(sockfd, dataServ, 2048, 0)) > 0){
  		int write_sz = fwrite(dataServ, sizeof(char), sbp, fr);
  	   if(write_sz < sbp){
  	    printf("File write failed.\n");
  	  }
      bzero(dataServ, 2048);
  		if (sbp == 0 || sbp != 2048) {
  			break;
  		}
		}
		if(sbp < 0){
			if (errno == EAGAIN) {
				printf("recv() timed out.\n");
			} else {
				printf( "recv() failed due to errno = %d\n");
			}
		}
//	  printf("Ok received from server!\n");
	  fclose(fr);
	}

//    printf("End\n");

  close(sockfd);
  
  int ret;
  ret = remove("client.csv");
  return 0;


}
