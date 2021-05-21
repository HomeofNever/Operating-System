/* fork-buffering.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  printf( "maximum buffer size is: %d\n", _IO_BUFSIZ );

/* setvbuf( stdout, NULL, _IONBF, 0 ); */

  int rc;
  printf( "ONE-%d\n", getpid() );
/* fflush( stdout );   <== for Homework 2 -- also before execl() */
  rc = fork();
  printf( "TWO-%d\n", getpid() );

  if ( rc == 0 ) { printf( "THREE-%d\n", getpid() ); }
  if ( rc > 0 ) { printf( "FOUR-%d\n", getpid() ); }

  return EXIT_SUCCESS;
}

/* OUTPUT POSSIBILITIES WHEN REDIRECTED TO FILE:

bash$ ./a.out > output.txt ; cat output.txt
ONE-19621
TWO-19621
FOUR-19621
ONE-19621
TWO-19622
THREE-19622

bash$ ./a.out > output.txt ; cat output.txt
ONE-19621
TWO-19622
THREE-19622
ONE-19621
TWO-19621
FOUR-19621

*/
