
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define min(a, b) ((a) < (b)) ? ((a)) : ((b))

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
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];

   sockfd=socket(AF_INET,SOCK_STREAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(SER2NET_HOST);
   servaddr.sin_port=htons(2001);

   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
   sendto(sockfd, "ollpera\n", strlen("ollpera\n"), 0, 
	(struct sockaddr *)&servaddr,sizeof(servaddr));
   while(1) //FIXME endless LOOP
   {
      n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stderr);
   }
}
 
int main(void)
{
  int lamp=0;
  int status=0;
  /* Extract the given parameter */
  extractArguments(&lamp, &status);

  changeLampState(lamp, status);

  printf( "Content-Type: text/plain\n\n" );
  printf("Hello from the CGI!\n");
  printf("LAMP=%d, STATUS=%d\n", lamp, status); //FIXME remove debugcode later

  return 0;
}
