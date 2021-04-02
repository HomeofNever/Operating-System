#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

extern int next_thread_id;
extern int max_squares;
extern char ***dead_end_boards;

// Lock written when modified these vars
pthread_mutex_t dead_end_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t next_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t max_squares_mutex = PTHREAD_MUTEX_INITIALIZER;

// Dead map counter
int num_of_alloc_dead_boards = 8;
int num_of_dead_boards = 0;

// Map info
int m = 0;
int n = 0;
// Display at least min squares covered
int min = 0;

// CONST
char EMPTY_CELL = '.';
char PASSED_CELL = 'S';
enum THREAD_TYPE {
    MAIN, THREAD
};

struct pack {
    char **map;
    int current_step;
    int x;
    int y;
    int thread_id;
};

struct joinPack {
    int thread_id;
    int current_step;
};

/**
 * Generic error message
 */
int error() {
    fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <m> <n> <r> <c> <x>\n");
    return EXIT_FAILURE;
}

char **copyMap(char **map) {
    char **new_map = calloc(m, sizeof(char *));
    for (int i = 0; i < m; i++) {
        *(new_map + i) = calloc(n, sizeof(char));
        for (int j = 0; j < n; j++) {
            *(*(new_map + i) + j) = map[i][j];
        }
    }

    return new_map;
}

void updateSquareCount(int s) {
    pthread_mutex_lock(&max_squares_mutex);
    if (s > max_squares) {
        max_squares = s;
    }
    pthread_mutex_unlock(&max_squares_mutex);
}

void writeDeadMap(struct pack *p) {
    pthread_mutex_lock(&dead_end_mutex);
    if (p->current_step >= min) {
        if (num_of_dead_boards >= num_of_alloc_dead_boards) {
            // Need to reallocate and copy
            int next = num_of_alloc_dead_boards * 2;
            dead_end_boards = realloc(dead_end_boards, next * sizeof(char **));
            num_of_alloc_dead_boards = next;
        }

        *(dead_end_boards + num_of_dead_boards) = copyMap(p->map);
        num_of_dead_boards++;
    }
    pthread_mutex_unlock(&dead_end_mutex);

    updateSquareCount(p->current_step);
}

int getNextThreadId() {
    pthread_mutex_lock(&next_thread_mutex);
    int thread_id = next_thread_id;
    next_thread_id++;
    pthread_mutex_unlock(&next_thread_mutex);
    return thread_id;
}

void freeMap(char **map) {
    for (int i = 0; i < m; i++) {
        free(*(map + i));
    }
    free(map);
}

void freeStep(int **steps) {
    for (int i = 0; i < 8; i++) {
        free(*(steps + i));
    }
    free(steps);
}

int isValidBlock(int x, int y, char **map) {
    if (x < 0 || y < 0) {
        return 0;
    }
    if (x >= m || y >= n) {
        return 0;
    }

    if (*(*(map + x) + y) != EMPTY_CELL) {
        return 0;
    }

    return 1;
}

void addStep(int x, int y, char **map, int *count, int **steps) {
    if (isValidBlock(x, y, map)) {
        **(steps + *count) = x;
        *(*(steps + *count) + 1) = y;
        *count += 1;
    }
}

void nextStep(struct pack *p, int *count, int **steps) {
    // Always allocate 8 directions with 2 coordinates
    // but we may not be able to use them all
    int x = p->x;
    int y = p->y;
    char **map = p->map;
    *count = 0;

    // up 2 left 1
    addStep(x - 2, y - 1, map, count, steps);
    // up 2 right 1
    addStep(x - 2, y + 1, map, count, steps);
    // right 2 up 1
    addStep(x - 1, y + 2, map, count, steps);
    // right 2 down 1
    addStep(x + 1, y + 2, map, count, steps);
    // down 2 right 1
    addStep(x + 2, y + 1, map, count, steps);
    // down 2 left 1
    addStep(x + 2, y - 1, map, count, steps);
    // left 2 down 1
    addStep(x + 1, y - 2, map, count, steps);
    // left 2 up 1
    addStep(x - 1, y - 2, map, count, steps);

}

void printBoard(char **map) {
    for (int i = 0; i < m; i++) {
        printf("MAIN: ");
        if (i == 0) {
            printf(">>");
        } else {
            printf("  ");
        }

        for (int j = 0; j < n; j++) {
            printf("%c", *(*(map + i) + j));
        }

        if (i == m - 1) {
            printf("<<\n");
        } else {
            printf("\n");
        }
    }
}

int isFullTour(char **map) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (*(*(map + i) + j) == EMPTY_CELL) {
                return 0;
            }
        }
    }

    return 1;
}

void *worker(void *mp) {
    struct pack *p = (struct pack *) mp;

    // mark current step
    *(*(p->map + p->x) + p->y) = PASSED_CELL;

    // calculate step
    int **steps;
    int count = 0;
    steps = calloc(8, sizeof(int *));
    for (int i = 0; i < 8; i++) {
        *(steps + i) = calloc(2, sizeof(int));
    }
    nextStep(p, &count, steps);

    if (count > 1) {
        // We need children, pack info and send!
        if (p->thread_id == -1) {
            // This is dispatch from Main thread, considered as Main
            printf("MAIN: %d possible moves after move #%d; creating %d child threads...\n", count, p->current_step,
                   count);
        } else {
            printf("THREAD %d: %d possible moves after move #%d; creating %d child threads...\n", p->thread_id,
                   count, p->current_step, count);
        }

        pthread_t *tid = calloc(count, sizeof(pthread_t));
        struct pack *pp = calloc(count, sizeof(struct pack));
        // Summary from children
        int max_step = 0;
        struct joinPack *joinPack;

        // Starting Children
        for (int i = 0; i < count; i++) {
            (pp + i)->thread_id = getNextThreadId();
            (pp + i)->map = copyMap(p->map);
            (pp + i)->current_step = p->current_step + 1;
            (pp + i)->x = **(steps + i);
            (pp + i)->y = *(*(steps + i) + 1);
            pthread_create(tid + i, NULL, worker, pp + i);
#ifdef NO_PARALLEL
            pthread_join(*(tid + i), (void *) &joinPack);
            if (max_step < joinPack->current_step) {
                max_step = joinPack->current_step;
            }
            if (p->thread_id == -1) {
                printf("MAIN: Thread %d joined (returned %d)\n", joinPack->thread_id,
                       joinPack->current_step);
            } else {
                printf("THREAD %d: Thread %d joined (returned %d)\n", p->thread_id, joinPack->thread_id,
                       joinPack->current_step);
            }
            free(joinPack);
#endif
        }
#ifndef NO_PARALLEL
        for (int i = 0; i < count; i++) {
            pthread_join(*(tid + i), (void *) &joinPack);
            if (max_step < joinPack->current_step)
                max_step = joinPack->current_step;
            if (p->thread_id == -1) {
                printf("MAIN: Thread %d joined (returned %d)\n", joinPack->thread_id,
                       joinPack->current_step);
            } else {
                printf("THREAD %d: Thread %d joined (returned %d)\n", p->thread_id, joinPack->thread_id,
                       joinPack->current_step);
            }
            free(joinPack);
        }
#endif
        free(pp);
        struct joinPack *jp = calloc(1, sizeof(struct joinPack));
        jp->thread_id = p->thread_id;
        jp->current_step = max_step;
        free(tid);
        freeStep(steps);
        freeMap(p->map);
        return jp;
    } else if (count == 1) {
        // Single solution, so we don't dispatch and keep going!
        struct pack pp = {p->map, p->current_step + 1, **steps, *(*steps + 1), p->thread_id};
        freeStep(steps);
        return worker(&pp);
        // reuse the map
    } else {
        // Dead end or... we find the tour?
        if (isFullTour(p->map)) {
            printf("THREAD %d: Sonny found a full knight's tour\n", p->thread_id);
            updateSquareCount(p->current_step);
        } else {
            if (p->thread_id == -1) {
                printf("MAIN: Dead end at move #%d\n", p->current_step);
            } else {
                printf("THREAD %d: Dead end at move #%d\n", p->thread_id, p->current_step);
            }
            // Write Dead Board
            writeDeadMap(p);
        }
        struct joinPack *jp = calloc(1, sizeof(struct joinPack));
        jp->thread_id = p->thread_id;
        jp->current_step = p->current_step;
        freeStep(steps);
        freeMap(p->map);
        return jp;
    }
}

/**
 * ERROR: Invalid argument(s)
 * USAGE: a.out <m> <n> <r> <c> <x>
 **/
int simulate(int argc, char *argv[]) {
    if (argc < 6) {
        error();
    }

    // Map size
    m = atoi(*(argv + 1));
    n = atoi(*(argv + 2));
    if (m < 2 || n < 2)
        return error();

    // Initial Location
    int r = atoi(*(argv + 3));
    int c = atoi(*(argv + 4));
    if (r >= m || c >= n || r < 0 || c < 0)
        return error();

    // Print Size for dead end
    min = atoi(*(argv + 5));
    if (min < 1 || min > m * n)
        return error();

    // init map
    char **map = calloc(m, sizeof(char *));
    for (int i = 0; i < m; i++) {
        *(map + i) = calloc(n, sizeof(char));
        for (int j = 0; j < n; j++) {
            *(*(map + i) + j) = EMPTY_CELL;
        }
    }

    printf("MAIN: Solving Sonny's knight's tour problem for a %dx%d board\n", m, n);
    printf("MAIN: Sonny starts at row %d and column %d\n", r, c);
    struct pack init = {map, 1, r, c, -1}; // -1 for identify main thread


    struct joinPack *j = worker(&init);
    free(j);

    // Summary
    int all = m * n;
    if (max_squares == all) {
        printf("MAIN: All threads joined; full knight's tour of %d achieved\n", all);
    } else {
        printf("MAIN: All threads joined; best solution(s) visited %d square%sout of %d\n", max_squares,
               max_squares == 1 ? " " : "s ", all);
        printf("MAIN: Dead end board%scovering at least %d square%s%s:\n",
               num_of_dead_boards == 1 ? " " : "s ", min,
               min == 1 ? " " : "s ",
               num_of_dead_boards == 1 ? "is" : "are");
        for (int i = 0; i < num_of_dead_boards; i++) {
            printBoard(dead_end_boards[i]);
        }
    }

    // Submitty check: do not free when submit, for local check please do
#ifdef LOCAL_MACHINE
    for (int i = 0; i < num_of_dead_boards; i++) {
        freeMap(dead_end_boards[i]);
    }
    free(dead_end_boards);
#endif

    return EXIT_SUCCESS;
}

