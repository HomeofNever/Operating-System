/* sizeof.c */

#include <stdio.h>
#include <stdlib.h>

int main()
{
  char c = 'c';
  unsigned int x = 13;
  short s = 2;
  long l = 123456789123456789;   /* long int l; */
  float f = 3.14159;
  double lf = 3.14159;

  printf( "sizeof char is %lu\n", sizeof( c ) );
  printf( "sizeof char is %lu\n", sizeof( char ) );

  printf( "sizeof int is %lu\n", sizeof( x ) );
  printf( "sizeof short is %lu\n", sizeof( s ) );
  printf( "sizeof long is %lu\n", sizeof( l ) );
  printf( "sizeof float is %lu\n", sizeof( f ) );
  printf( "sizeof double is %lu\n", sizeof( lf ) );

  printf( "sizeof void * is %lu\n", sizeof( void * ) );
  printf( "sizeof char * is %lu\n", sizeof( char * ) );
  printf( "sizeof int * is %lu\n", sizeof( int * ) );
  printf( "sizeof short * is %lu\n", sizeof( short * ) );
  printf( "sizeof long * is %lu\n", sizeof( long * ) );
  printf( "sizeof float * is %lu\n", sizeof( float * ) );
  printf( "sizeof double * is %lu\n", sizeof( double * ) );

  printf( "address of char c is %p\n", &c );
  printf( "address of int x is %p\n", &x );
  printf( "address of short s is %p\n", &s );
  printf( "address of long l is %p\n", &l );
  printf( "address of float f is %p\n", &f );
  printf( "address of double lf is %p\n", &lf );

  return EXIT_SUCCESS;
}
