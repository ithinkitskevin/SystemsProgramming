#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sendfile.h>

#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


#define MAX_CLIENT 4

void initalize();
char *perItoa(int num, char *str);
void *connection_handler(void *argument);
int poolCheck(int csd);

FILE* tempCSV;
FILE* finalCSV;
FILE* sendFinal;
char *searchForPara;




/*
server workflow
socket() -> Bind() -> listen() -> send()
*/
