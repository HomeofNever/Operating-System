/* forked.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* implement these functions in fork-functions.c */
int lecex2_child();
int lecex2_parent();

int main()
{
  int rc;

  /* create a new (child) process */
  pid_t p = fork();

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    rc = lecex2_child();
  }
  else /* p > 0 */
  {
    rc = lecex2_parent();
  }

  return rc;
}
