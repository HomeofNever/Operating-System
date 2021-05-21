/* command-line-args.c */

#include <stdio.h>
#include <stdlib.h>

                 /* char ** argv */
int main( int argc, char * argv[]  )
{
  printf( "argc is %d\n", argc );    /* argument count */
  printf( "argv[0] is %s\n", argv[0] );

  if ( argc != 4 )
  {
    fprintf( stderr, "ERROR: Invalid arguments\n" );
    fprintf( stderr, "USAGE: %s <filename> <x> <y>\n", argv[0] );
    return EXIT_FAILURE;
  }

  printf( "argv[1] is %s\n", *(argv+1) );
  printf( "argv[2] is %s\n", argv[2] );
  printf( "argv[3] is %s\n", argv[3] );

#if 0
                 char*                   Where is this allocated???
               +------+                   Generally on the runtime stack
  argv ---> [0]|  =======>"./a.out"        (see URL below...)
               +------+
            [1]|  =======>"abcd"         TO DO: Can argv[i] be modified?
               +------+
            [2]|  =======>"1234"                argv[1][2] = 'x';
               +------+
            [3]|  =======>"xyz"
               +------+

==> https://stackoverflow.com/questions/18681078/in-which-memory-segment-command-line-arguments-get-stored

#endif

  return EXIT_SUCCESS;
}
