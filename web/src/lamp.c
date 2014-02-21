#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define min(a, b) ((a) < (b)) ? ((a)) : ((b))

#define LOW		'l'
#define	HIGH		'h'

#define SER2NET_HOST	"127.0.0.1"

#define INPUTLAMP	"lamp="
#define INPUTSTATE	"value="

void extractArguments(int* lamp, char* status)
{
  char buffer[512]; 
  int inputLength = 0;
  char* offset;
 
  if ( getenv("CONTENT_LENGTH") )
  {
    inputLength = atoi( getenv("CONTENT_LENGTH") );
    fprintf(stderr, "CONTENT_LENGTH=%d\n", inputLength);
    inputLength = min( inputLength, sizeof(buffer)-1 ); /* Avoid buffer overflow */ 
    fread( buffer, inputLength, 1, stdin );

    fprintf(stderr, "Content: %s\n", buffer);
    offset = strstr(buffer, INPUTLAMP);
    if (offset)
	*lamp = atoi(offset + strlen(INPUTLAMP));
    
    offset = strstr(buffer, INPUTSTATE);
    if (offset)
	*status = *(offset + strlen(INPUTSTATE));


  }
}

void changeLampState(int lamp, int status)
{
   int sockfd,n;
   int offset=0;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];
   char* states=0;
   char* end = 0;
   int lampstate=0;

   sockfd=socket(AF_INET,SOCK_STREAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(SER2NET_HOST);
   servaddr.sin_port=htons(2001);
   
   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
   if ((char) status == ' ') /* Display only the actual status of the lamps */
   {
      sprintf (sendline, "ollpera\n");
   }
   else
   {
      sprintf (sendline, "ollpew%d%c\n", lamp, (char) status);
   }
   fprintf(stderr, "Send over TCP: %s", sendline);
   sendto(sockfd, sendline, strlen(sendline), 0, 
	(struct sockaddr *)&servaddr,sizeof(servaddr));
   offset=0;
   /* Opens the communication until the uC answers with an ACK */
   while( !strstr(recvline, "ACK") )
   {
      n=recvfrom(sockfd,recvline+offset,10000,0,NULL,NULL);
      offset+=n;
   }
   /*fprintf(stderr, "----------------\n");
   fputs(recvline,stderr);
   fprintf(stderr, "------  END -----\n"); */

   /* Extract an json with the actual status */
   states = strstr(recvline, "states");
   if (states)
   {
     /* skip states */
     states += strlen("states ");
     end = strchr(states, '\n'); 
     n = (int) (end - states);
     states[n]=0; /* insert an END instead of the newline */
     lampstate=atoi(states);
     fprintf(stderr, "state=%d\t[State=%s length=%d]\n", lampstate, states, n);
     if (lampstate)
     {
        printf("{\n");
	while (lampstate && n > 0)
	{
		printf("\"%d\": \"%c\"", n--, (lampstate % 10) ? 'h' : 'l');
		lampstate = lampstate / 10;
		if (n >= 1)
		{
		  printf(" ,");
		}
		 printf("\n");
	}
        printf("}\n");
     }
   }
}
 
int main(void)
{
  int lamp=0;
  char status=' ';
  /* Extract the given parameter */
  extractArguments(&lamp, &status);
  fprintf(stderr, "Got %dto %c\n", lamp, status);

  printf( "Content-Type: text/plain\n\n" );
  switch (status)
  {
  case HIGH:
  case LOW:
    changeLampState(lamp, status);
    break;
  default:
    changeLampState(0, ' ');
    break;
  }
  
  return 0;
}
