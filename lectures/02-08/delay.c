/* delay.c */

/* run this in the background:
 *
 * bash$ ./a.out &
 * [1] 25878            <=== what is that telling us?
 * bash$ 
 *
 * how do we know the background process has terminated?
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  printf( "PID %d: Calculating something very important...\n", getpid() );
  sleep( 10 );
  printf( "All done!  The answer is 42.\n" );

  return EXIT_SUCCESS;

  /* try changing this to other values */
  /* -- what is the range of values that we can return to the shell? */
}
