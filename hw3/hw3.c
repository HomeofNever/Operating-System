#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

extern int next_thread_id;
extern int max_squares;
extern char *** dead_end_boards;

void error() {
    fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <m> <n> <r> <c> <x>\n");
}

/**
 * ERROR: Invalid argument(s)
 * USAGE: a.out <m> <n> <r> <c> <x>
 **/
int simulate( int argc, char * argv[] ) {
    if (argc < 6) {
        error();
    }

    // Map size
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    if (m < 2 || n < 2) {
        error();
    }

    // Initial Location
    int r = atoi(argv[3]);
    int c = atoi(argv[4]);
    if (r > m || c > n || r < 0 || c < 0) {
        error();
    }

    // Print Size for dead end
    int x = atoi(argv[5]);
    if (x < 1 || x > m * n) {
        error();
    }

    



    return EXIT_SUCCESS;
}

