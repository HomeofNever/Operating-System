#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

int lecex3_q1_child(int pipe_read) {
    int shm_key, shm_size;
    if (read(pipe_read, &shm_key, sizeof(int)) > 0 && read(pipe_read, &shm_size, sizeof(int)) > 0) {
        int shmid = shmget( shm_key, shm_size, IPC_CREAT );
        if ( shmid == -1 )
        {
            perror( "shmget() failed" );
            return EXIT_FAILURE;
        }

        char * data = shmat( shmid, NULL, 0 );
        if ( data == (char *)-1 )
        {
            perror( "shmat() failed" );
            return EXIT_FAILURE;
        }

        for (int i = 0; i < shm_size; i++) {
            data[i] = toupper(data[i]);
        }

        int rc = shmdt( data );

        if ( rc == -1 )
        {   
            perror( "shmdt() failed" );
            exit( EXIT_FAILURE );
        }

        return EXIT_SUCCESS;
    }

    perror("Pipe read fail");
    return EXIT_FAILURE;
}