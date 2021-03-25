/* pthread-lecex3.c */

/*
 * Lecture Exercise 3 -- Practice Problem 6
 *
 * How many distinct possible outputs are there?
 *
 * Show all possible outputs.
 *
 * How many bytes are dynamically allocated on the heap?
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void * pthread_function( void * y )
{
  int * x = (int *)y;
  char * s = calloc( 16, sizeof( char ) );
  s[0] = ' ';
  strcat( s, "CHARMS" );
  for ( int i = 0 ; i < 8 ; i++ )
  {
    int tmp = *(x + i);
    tmp += i;
/*    usleep( 10000 ); */
    *(x + i) = tmp;
  }
  fprintf( stderr, "%s\n", s );
  return NULL;
}

int main()
{
  int * z = calloc( 8, sizeof( int ) );
  pthread_t t1, t2;
  pthread_create( &t1, NULL, pthread_function, z );
  pthread_create( &t2, NULL, pthread_function, z );
  fprintf( stderr, "LUCKY" );
  pthread_join( t2, NULL );
  pthread_join( t1, NULL );
  for ( int i = 0 ; i < 8 ; i++ ) printf( "%d%s", *(z + i), i == 7 ? "" : "-" );
  printf( "\n" );
  return EXIT_SUCCESS;
}

