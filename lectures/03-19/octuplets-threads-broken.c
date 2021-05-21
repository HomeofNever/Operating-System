/* octuplets-threads-broken.c */

/* Run this code to see what happens.....
 *
 * We will usually see all child threads get 16 as its input argument
 *  (to the whattodo() function) -- this occurs because each thread
 *   gets a pointer to the same statically allocated memory for
 *    variable t declared in main()
 *
 * Consider this an "anti-pattern" -- i.e., what not to do when
 *  creating multi-threaded code using pthreads in C....
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define CHILDREN 8

/* this function is called by each thread */
void * whattodo( void * arg );

int main()
{
  pthread_t tid[CHILDREN];    /* keep track of thread IDs */

  int i, rc, children = CHILDREN;

  for ( i = 0 ; i < children ; i++ )
  {
    int t = 2 + i * 2;   /* 2, 4, 6, 8, ... */
    printf( "MAIN: Next child thread will nap for %d seconds\n", t );

    rc = pthread_create( &tid[i], NULL, whattodo, &t );

    if ( rc != 0 )
    {
      fprintf( stderr, "pthread_create() failed (%d)\n", rc );
    }
  }

sleep( 20 );

  return EXIT_SUCCESS;
}


/* each child process will sleep for t second,    */
/*  then return t as exit status when it wakes up */
void * whattodo( void * arg )
{
  int t = *(int *)arg;

  printf( "THREAD %lu: I'm gonna nap for %d seconds\n", pthread_self(), t );
  sleep( t );
  printf( "THREAD %lu: I'm awake!\n", pthread_self() );

  return NULL;
}

