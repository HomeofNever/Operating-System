/* rlimit.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

int main()
{
  /* TO DO: capture and check the return values from get/setrlimit() */
  struct rlimit rl;
  getrlimit( RLIMIT_NPROC, &rl );
  printf( "RLIMIT_NPROC soft limit: %ld\n", rl.rlim_cur );
  printf( "RLIMIT_NPROC hard limit: %ld\n", rl.rlim_max );

  /* lower the RLIMIT_NPROC to 20 */
  rl.rlim_cur = 20;
  setrlimit( RLIMIT_NPROC, &rl );

  /* display the RLIMIT_NPROC values again */
  getrlimit( RLIMIT_NPROC, &rl );
  printf( "RLIMIT_NPROC soft limit: %ld\n", rl.rlim_cur );
  printf( "RLIMIT_NPROC hard limit: %ld\n", rl.rlim_max );

  while ( 1 )   /* change this to 1 only if the above code */
  {             /*  successfully changes the soft limit */
    int p = fork();

    if ( p == -1 )
    {
      perror( "fork() failed" );
      return EXIT_FAILURE;
    }

    printf( "PID %d: fork() worked!\n", getpid() );

    sleep( 3 );   /* this gives me time to shut all the processes down! */
  }

  return EXIT_SUCCESS;
}

