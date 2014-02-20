
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define min(a, b) ((a) < (b)) ? ((a)) : ((b))

#define LOW		'l'
#define	HIGH		'h'

#define SER2NET_HOST	"10.23.43.31"

void extractArguments(int* lamp, int* status)
{
  char buffer[512]; 
  int inputLength = 0;
  char charlength[5];  
 
  if ( getenv("CONTENT_LENGTH") )
  {
    inputLength = atoi( getenv("CONTENT_LENGTH") );
    fprintf(stderr, "CONTENT_LENGTH=%d\n", inputLength);
    inputLength = min( inputLength, sizeof(buffer)-1 ); /* Avoid buffer overflow */ 
    fread( buffer, inputLength, 1, stdin );

    fprintf(stderr, "Content: %s\n", buffer);
  }
}

void changeLampState(int lamp, int status)
{
   int sockfd,n;
   int offset=0;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];

   sockfd=socket(AF_INET,SOCK_STREAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(SER2NET_HOST);
   servaddr.sin_port=htons(2001);
   
   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  
   sprintf (sendline, "ollpew%d%c\n", lamp, (char) status);
   sendto(sockfd, sendline, strlen(sendline), 0, 
	(struct sockaddr *)&servaddr,sizeof(servaddr));
   offset=0;
   /* Opens the communication until the uC answers with an ACK */
   while( !strstr(recvline, "ACK") )
   {
      n=recvfrom(sockfd,recvline+offset,10000,0,NULL,NULL);
      offset+=n;
   }
   fprintf(stderr, "----------------\n");
   fputs(recvline,stderr);
   fprintf(stderr, "------  END -----\n");
}
 
int main(void)
{
  int lamp=0;
  int status=0;
  /* Extract the given parameter */
  extractArguments(&lamp, &status);

  changeLampState(2, LOW);

  printf( "Content-Type: text/plain\n\n" );
  printf("Hello from the CGI!\n");
  printf("LAMP=%d, STATUS=%d\n", lamp, status); //FIXME remove debugcode later

  return 0;
}
