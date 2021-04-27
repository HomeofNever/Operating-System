#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

extern int next_thread_id;
extern int max_squares;
extern char *** dead_end_boards;

int num_solutions;
int num_end_size;

/* global mutex variable */
pthread_mutex_t mutex_next_thread_id = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_squares = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_dead_end_boards = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int num;
    int* rs;
    int* cs;
} all_moves_t;

typedef struct {
    int r;
    int c;
    int m;
    int n;
    int x;
    int num_moves;
    int id;
    char** current_boards;
} child_start_t;

all_moves_t* find_all_moves(int r, int c, int m, int n, char** current_boards);
void next_move(int* num_moves, int r, int c, int m, int n, int x, char** current_boards, int id);
void* child_simulate(void* arg);
void output_board(char** board, int m, int n);
int simulate( int argc, char ** argv);

all_moves_t* find_all_moves(int r, int c, int m, int n, char** current_boards) {
    /* At most there are 8 possibilities */
    int new_r, new_c, i;
    all_moves_t* all_moves = calloc(1, sizeof(all_moves_t));
    all_moves->num = 0;
    all_moves->rs = calloc(8, sizeof(int));
    all_moves->cs = calloc(8, sizeof(int));
    int* d_r = calloc(8, sizeof(int));
    int* d_c = calloc(8, sizeof(int));
    *d_r = -2; *(d_r+1) = -2;
    *(d_r+2) = -1; *(d_r+3) = 1;
    *(d_r+4) = 2; *(d_r+5) = 2;
    *(d_r+6) = 1; *(d_r+7) = -1;
    *d_c = -1; *(d_c+1) = 1;
    *(d_c+2) = 2; *(d_c+3) = 2;
    *(d_c+4) = 1; *(d_c+5) = -1;
    *(d_c+6) = -2; *(d_c+7) = -2;

    for (i=0; i<8; i++) {
        new_r = r + *(d_r+i);
        new_c = c + *(d_c+i);
        /* Check if new_r and new_c are in the boundary also cannot be 'S' */
        if ((new_r>=0) && (new_r<=m-1) && (new_c>=0) && (new_c<=n-1) 
            && (*(*(current_boards+new_r)+new_c)) != 'S') {
            *((all_moves->rs)+(all_moves->num)) = new_r;
            *((all_moves->cs)+(all_moves->num)) = new_c;
            (all_moves->num)++;
        }
    }

    free(d_r); free(d_c);
    return all_moves;
}

void next_move(int* num_moves, int r, int c, int m, int n, int x, char** current_boards, int id) {
    char* current_thread = calloc(9+id, sizeof(char));
    if (id == 0)
        strcpy(current_thread, "MAIN");
    else 
        sprintf(current_thread, "THREAD %d", id);
    int num_children = 0, i, moves = 0;
    int* ids;
    pthread_t* tids;
    while (1) {
        all_moves_t* all_moves = find_all_moves(r, c, m, n, current_boards);
#ifdef DEBUG
    printf("\nDEBUG in %s----------------\n", current_thread);
    printf("all_moves: %d\n", all_moves->num);
    printf("----------------\n\n");
#endif
        if (all_moves->num == 0) {
            free(all_moves->rs);
            free(all_moves->cs);
            free(all_moves);
            break;
        }
        if (all_moves->num == 1) {
            r = *(all_moves->rs);
            c = *(all_moves->cs);
            *(*(current_boards+r)+c) = 'S';
        }
        else {
            printf("%s: %d possible moves after move #%d; creating %d child threads...\n",
                current_thread, all_moves->num, *num_moves, all_moves->num);
            tids = calloc(all_moves->num, sizeof(pthread_t));
            ids = calloc(all_moves->num, sizeof(int));
            for (i=0; i<all_moves->num; i++) {
                child_start_t* child_start_arg = calloc(1, sizeof(child_start_t));
                child_start_arg->m = m;
                child_start_arg->n = n;
                child_start_arg->x = x;
                child_start_arg->r = *((all_moves->rs)+i);
                child_start_arg->c = *((all_moves->cs)+i);
                child_start_arg->num_moves = *num_moves;
                pthread_mutex_lock( &mutex_next_thread_id );
                child_start_arg->id = next_thread_id;
                *(ids+i) = next_thread_id;
                next_thread_id++;
                pthread_mutex_unlock( &mutex_next_thread_id );
                child_start_arg->current_boards = current_boards;       
                pthread_t tid;
                int rc = pthread_create(&tid, NULL, child_simulate, child_start_arg);
                if (rc != 0)
                    fprintf(stderr, "ERROR: pthread_create() failed (%d): %s\n", rc, strerror(rc));
                
            #ifdef NO_PARALLEL
                int* possible_moves;
                pthread_join(tid, (void **)&possible_moves);
                printf("%s: Thread %d joined (returned %d)\n", current_thread, *(ids+i), *possible_moves);
                if (*possible_moves > moves) moves = *possible_moves;
                pthread_mutex_lock( &mutex_max_squares );
                if (*possible_moves > max_squares) {
                    max_squares = *possible_moves;
                }
                pthread_mutex_unlock( &mutex_max_squares );
                free(possible_moves);
            #endif

                *(tids+i) = tid;
            }
            num_children = all_moves->num;
            free(all_moves->rs);
            free(all_moves->cs);
            free(all_moves);
            break;
        }
        (*num_moves)++;
        free(all_moves->rs);
        free(all_moves->cs);
        free(all_moves);
    }

    if (num_children == 0) {
        if (*num_moves >= x) {
            pthread_mutex_lock( &mutex_dead_end_boards );
            if (num_solutions >= num_end_size) {
                num_end_size *= 2;
                dead_end_boards = realloc(dead_end_boards, num_end_size*sizeof(*dead_end_boards));
            }
            *(dead_end_boards+num_solutions) = current_boards; 
            num_solutions++;
            pthread_mutex_unlock( &mutex_dead_end_boards );
        }
        else {
            for (i=0; i<m; i++)
                free(*(current_boards+i));
            free(current_boards);
        }
        pthread_mutex_lock( &mutex_max_squares );
        if (*num_moves > max_squares) {
            max_squares = *num_moves;
        }
        pthread_mutex_unlock( &mutex_max_squares );
        if (*num_moves == m*n)
            printf("%s: Sonny found a full knight's tour!\n", current_thread);
        else
            printf("%s: Dead end at move #%d\n", current_thread, *num_moves);
    }
    if (num_children >= 2) {
    #ifndef NO_PARALLEL
        for (i=0; i<num_children; i++) {
            int* possible_moves;
            pthread_join(*(tids+i), (void **)&possible_moves);
            printf("%s: Thread %d joined (returned %d)\n", current_thread, *(ids+i), *possible_moves);
            if (*possible_moves > moves) moves = *possible_moves;
            pthread_mutex_lock( &mutex_max_squares );
            if (*possible_moves > max_squares) {
                max_squares = *possible_moves;
            }
            pthread_mutex_unlock( &mutex_max_squares );
            free(possible_moves);
        }
    #endif
        *num_moves = moves;
        for (i=0; i<m; i++)
            free(*(current_boards+i));
        free(current_boards);
        free(tids);
        free(ids);
    }

    free(current_thread);
}

void* child_simulate(void* arg) {
    child_start_t* child_start_arg = (child_start_t*) arg;

    int m = child_start_arg->m, n=child_start_arg->n, r = child_start_arg->r, c = child_start_arg->c,
        x = child_start_arg->x, id = child_start_arg->id;

    int* num_moves = calloc(1,sizeof(int)); 
    *num_moves = child_start_arg->num_moves;

    /* copy the boards */
    char** current_boards = calloc(m, sizeof(char*));
    int i;
    for (i=0; i<m; i++) {
        *(current_boards+i) = calloc(n, sizeof(char));
        int j;
        for (j=0; j<n; j++)
            *(*(current_boards+i)+j) = *(*(child_start_arg->current_boards+i)+j);
    }
    
    free(arg);

    (*num_moves)++;
    *(*(current_boards+r)+c) = 'S';
#ifdef DEBUG
    printf("\nDEBUG in THREAD %d----------------\n", tid);
    output_board(current_boards, m, n);
    printf("----------------\n\n");
#endif
    next_move(num_moves, r, c, m, n, x, current_boards, id);

    pthread_exit(num_moves);
}

void output_board(char** board, int m, int n) {
    int i, j;
    for (i=0; i<m; i++) {
        printf("MAIN: ");
        if (i==0)
            printf(">>");
        else 
            printf("  ");
        for (j=0; j<n; j++)
            printf("%c", *(*(board+i)+j));
        if (i == m-1)
            printf("<<\n");
        else 
            printf("\n");
    }   
}

int simulate( int argc, char ** argv) {
    num_solutions = 0, num_end_size = 8;
    // setvbuf( stdout, NULL, _IONBF, 0 );
    if (argc != 6) {
        fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <m> <n> <r> <c> <x>");
        return EXIT_FAILURE;
    }

    int m = atoi(*(argv+1)), n = atoi(*(argv+2)), 
    r = atoi(*(argv+3)), c = atoi(*(argv+4)), x = atoi(*(argv+5));
    if ((m<0) || (n<0) || (r<0) || (r>=m) || (c<0) || (c>=n) || (x>m*n)) {
        fprintf(stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <m> <n> <r> <c> <x>");
        return EXIT_FAILURE;
    }

    printf("MAIN: Solving Sonny's knight's tour problem for a %dx%d board\n", m, n);

    /* Initialize the boards */
    char** current_boards = calloc(m, sizeof(char*));
    int i;
    for (i=0; i<m; i++) {
        *(current_boards+i) = calloc(n, sizeof(char));
        int j;
        for (j=0; j<n; j++)
            *(*(current_boards+i)+j) = '.';
    }

    printf("MAIN: Sonny starts at row %d and column %d\n", r, c);
    *(*(current_boards+r)+c) = 'S';
    int* num_moves = calloc(1,sizeof(int));
    *num_moves = 1;

    next_move(num_moves, r, c, m, n, x, current_boards, 0);

    if (max_squares != m*n) {
        printf("MAIN: All threads joined; best solution(s) visited %d %s out of %d\n",
            max_squares, max_squares == 1 ? "square" : "squares", m*n);
        printf("MAIN: Dead end %s covering at least %d %s %s:\n", num_solutions == 1 ? "board" : "boards",
            x, x == 1 ? "square" : "squares", num_solutions == 1 ? "is" : "are");

        for (i=0; i<num_solutions; i++) 
            output_board(*(dead_end_boards+i), m, n);
    }
    else 
        printf("MAIN: All threads joined; full knight's tour of %d achieved\n", m*n);
    
    free(num_moves);

//     int j;
//     for (i=0; i<num_solutions; i++) {
//         for (j=0; j<m; j++)
//             free(*(*(dead_end_boards+i)+j));
//         free(*(dead_end_boards+i));
//     }
//     free(dead_end_boards);


    return EXIT_SUCCESS;
}
