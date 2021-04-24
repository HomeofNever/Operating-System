#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int currentLocation = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct runPack {
    char * filename;
    char * startPointer;
    int readOffset;
    int readNum;
};

void * worker(void *rp) {
    struct runPack * runp = (struct runPack *) rp;
    int fd = open(runp->filename, O_RDONLY);

    if (fd == -1) {
        perror("Error");
        fprintf(stderr, "read file error");
        return NULL;
    }

    lseek(fd, runp->readOffset, SEEK_CUR);

    char letter;
    int num = 0;
    char * offset = runp->startPointer;
    while (read(fd, &letter, 1) > 0 && num < 111) {
        if(isalnum(letter)) {
            pthread_mutex_lock( &mutex );
            int loc = currentLocation;
            currentLocation++;
            pthread_mutex_unlock( &mutex );
            offset[loc] = letter;
            num += 1;
        }
    }

    free(runp);
    return NULL;
}

int main(int argc, char * argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }

    key_t key = atoi(argv[1]);
    int shmid = shmget( key, 32768, 0 );
    void * pointer = shmat( shmid, NULL, 0 );

    char * filename = calloc(100, sizeof(char));
    strcpy(filename, ((char *) pointer) + 1);
    int filenameLength = strlen(filename);
    int * numThreads = (int *) (((char *) pointer) + 1 + filenameLength + 1);
    int * numOffsets = numThreads + 1;
    char * startPointer = (char *)(numOffsets + *numThreads);

    pthread_t *tid = calloc(*numThreads, sizeof(pthread_t));
    for (int i = 0; i < *numThreads; i++) {
        struct runPack * rp = calloc(1, sizeof(struct runPack));
        rp->filename = filename;
        rp->startPointer = startPointer;
        rp->readOffset = numOffsets[i];
        rp->readNum = 111;
        pthread_create(tid + i, NULL, worker, rp);
    }

    for (int i = 0; i < *numThreads; i++) {
        pthread_join(*(tid + i), NULL);
    }

    free(filename);
    char * success = (char *) pointer;
    success[0] = 'G';
    shmdt(pointer);

    return EXIT_SUCCESS;
}