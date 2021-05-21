/* buffering.c */

/* When printing to the terminal via stdout,
 *  a '\n' character will "flush" the buffer,
 *   i.e., output everything that has been stored
 *    in the output buffer so far
 * ==> this is called line-based buffering
 *
 * When we output to a file...
 *
 *    ./a.out > output.txt
 *
 * ...the '\n' character no longer flushes the
 *     output buffer
 * ==> this is called block-based buffering
 *
 *
 * also check out fflush()
 *
 *    fflush( stdout );
 */

#include <stdio.h>
#include <stdlib.h>

int main()
{
#if 0
  /* change stdout buffering to be unbuffered: */
  setvbuf( stdout, NULL, _IONBF, 0 );
#endif

  /* stderr is always unbuffered, meaning that   */
  /*  we will always immediately see this output */
  fprintf( stderr, "OUTPUT!" );

  printf( "HERE0\n" );
  int * x = NULL;
                       /* (no newlines in HERE0 or HERE1) */
                       /* (also, output to the terminal)  */
  printf( "HERE1" );   /* OS buffer: "HERE0HERE1"         */
  *x = 1234;

                       /* (one newline in HERE0\n)  */
                       /* (also, output to a file)  */
                       /* OS buffer: "HERE0\nHERE1" */


  printf( "HERE2" );
  printf( "x points to %d\n", *x );

  /* TO DO: Any and all debugging output statements */
  /*         should end in newline '\n' characters  */

  return EXIT_SUCCESS;
}
