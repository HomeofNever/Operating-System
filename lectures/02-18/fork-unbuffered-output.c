/* fork-unbuffered-output.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void output( char * s )   /* s: "CHILD: good-bye!\n" */
{                         /*      ^                  */
                          /*     ptr                 */

  setbuf( stdout, NULL );    /* set stdout to be unbuffered */

  char * ptr = s;
  int c;
  while ( ( c = *ptr++ ) ) putc( c, stdout );
}


int main()
{
  pid_t p = fork();

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    output( "CHILD: happy birthday to me!\n" );
    output( "CHILD: good-bye!\n" );
  }
  else /* p > 0 */
  {
    output( "PARENT: child process created.\n" );
    output( "PARENT: bye!\n" );
  }

  return EXIT_SUCCESS;
}
