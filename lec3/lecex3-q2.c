/* octuplets-threads.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

void * copy_file( void * arg );

int main(int argc, char **argv)
{
    pthread_t tid[argc - 1]; /* keep track of thread IDs */

    for (int i = 1; i < argc; i++)
    {
        printf("MAIN: Creating thread to copy \"%s\"\n", argv[i]);
        int rc = pthread_create(&tid[i - 1], NULL, copy_file, argv[i]);
        if (rc != 0)
        {
            fprintf(stderr, "pthread_create() failed (%d)\n", rc);
        }
    }

    /* wait for child threads to terminate */
    int count = 0;
    for (int i = 0; i < argc - 1; i++)
    {
        int *x;
        pthread_join(tid[i], (void **)&x); /* BLOCK */
        count += *x;
        printf("MAIN: Thread completed copying %d bytes for \"%s\"\n", *x, argv[i + 1]);
        free(x);
    }

    printf("MAIN: Successfully copied %d bytes via %d child thread%s", count, argc - 1, argc - 1 == 1 ? "\n" : "s\n");

    return EXIT_SUCCESS;
}