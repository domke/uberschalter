#include "stdio.h"
 
int main(void) {
  printf( "Content-Type: text/plain\n\n" );
  printf("Hello from the CGI!\n");
  fprintf(stderr, "Debug\n");
  return 0;
}
