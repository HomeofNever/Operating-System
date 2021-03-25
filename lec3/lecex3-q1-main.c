/* lecex3-q1-main.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hidden-header-file.h"

int lecex3_q1_parent( int pipefd );
int lecex3_q1_child( int pipefd );

int main()
{
  int p[2];
  int pipe_rc = pipe( p );
  if ( pipe_rc == -1 ) { perror( "pipe() failed" ); return EXIT_FAILURE; }

  pid_t rc = fork();
  if ( rc == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

  if ( rc > 0 )
  {
    close( p[0] );  /* close the read end of the pipe */
    rc = lecex3_q1_parent( p[1] );
  }
  else if ( rc == 0 )
  {
    close( p[1] );  /* close the write end of the pipe */
    rc = lecex3_q1_child( p[0] );
  }

  return rc;
}
