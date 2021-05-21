/* static-allocation.c */

/* compile this code from bash shell (or terminal):

   bash$ gcc -Wall -Werror static-allocation.c
              ^^^^  ^^^^^^
         once successfully compiled, execute this code as follows:

   bash$ ./a.out
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int x = 5;  /* x is statically allocated memory
                (4 bytes are allocated on the stack) */

  printf( "x is %d\n", x );
  printf( "sizeof int is %ld\n", sizeof( int ) );


  int * y = NULL;  /* y is statically allocated memory
                      (8 bytes are allocated on the stack);
                       this will (hopefully) point to a
                        valid memory address.... */

  y = &x;  /* address-of operator is & */
  printf( "y points to %d\n", *y );

  /* use this in the shell to see debugging statements:

      bash$ gcc -Wall -Werror -D DEBUG_MODE static-allocation.c
                              ^^^^^^^^^^^^^
   */

#ifdef DEBUG_MODE
  printf( "sizeof int * is %ld\n", sizeof( int * ) );
  printf( "sizeof char * is %ld\n", sizeof( char * ) );
  printf( "sizeof double * is %ld\n", sizeof( double * ) );
  printf( "sizeof void * is %ld\n", sizeof( void * ) );
#endif


#if 0
  char * cptr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";  /* '\0' */

  cptr[2] = 'Q';
  /* we cannot do this assignment statement because
      cptr points to read-only memory (i.e., the code/data
       segment of the process memory space) */

  printf( "cptr points to \"%s\"\n", cptr );

  /* to do: try replacing char * cptr with a more specific
             static memory allocation -- char cptr[27]; */
#endif


  char name[5] = "David";

  printf( "Hi, %s\n", name );

  char xyz[5] = "QRSTU";

  printf( "Hi again, %s\n", name );

  /* correct the above code by allocating [6] instead of [5] */

  printf ( "xyz is %s\n", xyz );

  return EXIT_SUCCESS;   /* 0 */
}