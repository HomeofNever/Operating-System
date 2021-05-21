/* signals.c */

/* check out: "man 2 signal"
              "man 7 signal"
              "man sigaction" */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler( int sig )
{
  printf( "Rcvd signal %d\n", sig );

  if ( sig == SIGINT )
  {
    printf( "Stop hitting CTRL-C and answer the question!\n" );
  }
  else if ( sig == SIGUSR1 )
  {
    printf( "Pretending I'm a server, I'm going to reload my config file\n" );
  }
}

int main()
{
  /* TO DO: check the return value for signal() */

  signal( SIGINT, SIG_IGN );   /* ignore SIGINT (CTRL-C) */

  /* redefine SIGINT to call signal_handler() */
  signal( SIGINT, signal_handler );

  /* redefine SIGUSR1 to call signal_handler() */
  signal( SIGUSR1, signal_handler );

  char name[100];
  printf( "Enter your name: " );
  scanf( "%s", name );   /* BUFFER OVERFLOW POSSIBLE HERE! */
  printf( "Hi, %s\n", name );

  /* restore the default behavior/disposition for SIGINT */
  signal( SIGINT, SIG_DFL );

  /* ... */

  return EXIT_SUCCESS;
}
