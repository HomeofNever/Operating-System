/* fork-lexec2.c */

/*
 * Lecture Exercise 2 -- Practice Problem 4
 *
 * How many distinct possible outputs are there?
 *
 * Show all possible outputs.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int t;
  int * q = &t;
  t = 19;
  pid_t p = fork();

  /* for Lecture Exercise 2, assume fork() succeeds */
  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    printf( "CHILD: happy birthday to me!\n" );
    printf( "CHILD: *q is %d\n", *q );
  }
  else /* p > 0 */
  {
    printf( "PARENT: child process created\n" );
    *q = 73;
    printf( "PARENT: *q is %d\n", *q );
  }

  return EXIT_SUCCESS;
}
