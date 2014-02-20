
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) ((a) < (b)) ? ((a)) : ((b))

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

 
int main(void)
{
  int lamp=0;
  int status=0;
  /* Extract the given parameter */
  extractArguments(&lamp, &status);

  printf( "Content-Type: text/plain\n\n" );
  printf("Hello from the CGI!\n");
  printf("LAMP=%d, STATUS=%d\n", lamp, status); //FIXME remove debugcode later

  return 0;
}
