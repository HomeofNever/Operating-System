/* hw3-main.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int next_thread_id;        /* initialize to 1 */

int max_squares;           /* initialize to 0 */

char *** dead_end_boards;  /* initialize as array of NULL pointers of size 8 */

/* write the simulate() function and place all of your code in hw3.c */
int simulate( int argc, char * argv[] );

int main( int argc, char * argv[] )
{
  next_thread_id = 1;
  max_squares = 0;
  dead_end_boards = calloc( 8, sizeof( char ** ) );
  if ( dead_end_boards == NULL ) { perror( "calloc() failed" ); return EXIT_FAILURE; }
  int rc = simulate( argc, argv );

  /* on Submitty, there will likely be more code here that validates the
      global variables at this point... */

  return rc;
}

#if 0
  bash$ a.out 3 4 0 0 4

                      S...
                      ....
                      ....
                      /  \
            THREAD 1 /    \ THREAD 2
                    /      \
                S3..        S...
                ..1.        ....
                2...        .1..
                /  \
      THREAD 3 /    \ THREAD 4
              /      \
          S36.        S3..
          ..14        ..1.
          25..        2.4.
          /  \
THREAD 5 /    \ THREAD 6
        /      \
    S369        S369
    b814        7a14
    25a7        258b

#endif

