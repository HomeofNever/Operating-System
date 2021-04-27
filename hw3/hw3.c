#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
int m = 0; // row
int n = 0; // col
int full_step = 0; // m * n
// Display at least min squares covered
int min = 0;

// CONST
char EMPTY_CELL = '.';
char PASSED_CELL = 'S';

// Pass states to next thread/self
struct pack {
    char **map;
    int current_step;
    int x;
    int y;
    int thread_id;
};

// Return/join value
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

/**
 * Copy given map and return the pointer
 * @param map map to copy
 * @return a new allocated map pointer
 */
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

/**
 * Update max square
 * will identify if an update is required
 * thread-safe
 * @param s square count
 */
void updateSquareCount(int s) {
    pthread_mutex_lock(&max_squares_mutex);
    if (s > max_squares) {
        max_squares = s;
    }
    pthread_mutex_unlock(&max_squares_mutex);
}

/**
 * Write an dead map
 * will identify if map need to be written
 * @param p pack step info
 */
void writeDeadMap(struct pack *p) {
    if (p->current_step >= min) {
        pthread_mutex_lock(&dead_end_mutex);
        if (num_of_dead_boards >= num_of_alloc_dead_boards) {
            // Need to reallocate and copy
            int next = num_of_alloc_dead_boards * 2;
            dead_end_boards = realloc(dead_end_boards, next * sizeof(char **));
            num_of_alloc_dead_boards = next;
        }

        *(dead_end_boards + num_of_dead_boards) = copyMap(p->map);
        num_of_dead_boards++;
        pthread_mutex_unlock(&dead_end_mutex);
    }

    updateSquareCount(p->current_step);
}

/**
 * Get next thread id to allocate
 * thread safe
 * @return int next id to use
 */
int getNextThreadId() {
    pthread_mutex_lock(&next_thread_mutex);
    int thread_id = next_thread_id;
    next_thread_id++;
    pthread_mutex_unlock(&next_thread_mutex);
    return thread_id;
}

/**
 * Free a given map
 * @param map map pointer to free
 */
void freeMap(char **map) {
    for (int i = 0; i < m; i++) {
        free(*(map + i));
    }
    free(map);
}

/**
 * Free an immediate calculated step pointer
 * by nextStep()
 * @param steps
 */
void freeStep(int **steps) {
    for (int i = 0; i < 8; i++) {
        free(*(steps + i));
    }
    free(steps);
}

/**
 * Identify if given block is okay to go
 * @param x row
 * @param y col
 * @param map map to test
 * @return 1 if not passed, 0 otherwise
 */
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

/**
 * Immediate function with next step
 * @param x row
 * @param y col
 * @param map map to test
 * @param count num of step, indicator of steps
 * @param steps step info
 */
void addStep(int x, int y, char **map, int *count, int **steps) {
    if (isValidBlock(x, y, map)) {
        **(steps + *count) = x;
        *(*(steps + *count) + 1) = y;
        *count += 1;
    }
}

/**
 * Calculate possibly next step
 * @param p step info to calculate
 * @param count number of possible step
 * @param steps [[x, y], ...] next step location
 */
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

/**
 * Print a given map
 * should always for main thread
 * @param map map to print
 */
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

void *worker(void *mp) {
    struct pack *p = (struct pack *) mp;

    // mark current step
    *(*(p->map + p->x) + p->y) = PASSED_CELL;

    // allocate and calculate step
    int **steps = calloc(8, sizeof(int *));
    int count = 0;
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
        return jp; // The same as pthread_exit(jp)
    } else if (count == 1) {
        // Single solution, so we don't dispatch and keep going!
        struct pack pp = {p->map, p->current_step + 1, **steps, *(*steps + 1), p->thread_id};
        freeStep(steps);
        return worker(&pp);
        // reuse the map
    } else {
        // Dead end or... we find the tour?
        if (p->current_step == full_step) {
            printf("THREAD %d: Sonny found a full knight's tour!\n", p->thread_id);
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
    full_step = m * n;
    char **map = calloc(m, sizeof(char *));
    for (int i = 0; i < m; i++) {
        *(map + i) = calloc(n, sizeof(char));
        for (int j = 0; j < n; j++) {
            *(*(map + i) + j) = EMPTY_CELL;
        }
    }

    printf("MAIN: Solving Sonny's knight's tour problem for a %dx%d board\n", m, n);
    printf("MAIN: Sonny starts at row %d and column %d\n", r, c);
    struct pack init = {map, 1, r, c, -1}; // -1 to identify main thread


    struct joinPack *j = worker(&init);
    free(j);
    // map will always be freed by the worker

    // Summary
    if (max_squares == full_step) {
        printf("MAIN: All threads joined; full knight's tour of %d achieved\n", full_step);
    } else {
        printf("MAIN: All threads joined; best solution(s) visited %d square%sout of %d\n", max_squares,
               max_squares == 1 ? " " : "s ", full_step);
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

