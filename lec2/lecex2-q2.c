/* fork.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    setvbuf( stdout, NULL, _IONBF, 0 ); // for submitty ONLY
    pid_t pid; /* process id (pid) -- unsigned short */
    printf("PARENT: okay, start here.\n");

    /* create a new (child) process */
    pid = fork();
    if (pid == -1)
    {
        perror("fork() failed");
        return EXIT_FAILURE;
    }

    if (pid > 0)
    {
        /* the PID of the child process is returned in the parent */
        int status;

        waitpid(pid, &status, 0);

        pid = fork();
        if (pid == -1)
        {
            perror("fork() failed");
            return EXIT_FAILURE;
        }

        if (pid > 0)
        {
            waitpid(pid, &status, 0);
            printf("PARENT: both child processes terminated successfully.\n");
            printf("PARENT: phew, i'm glad they're gone!\n");
        }
        else if (pid == 0)
        {
            printf("CHILD B: and happy birthday to me!\n");
            printf("CHILD B: see ya later....self-terminating!\n");
        }
    }
    else if (pid == 0)
    {
        printf("CHILD A: happy birthday to me!\n");
        printf("CHILD A: i'm bored....self-terminating....good-bye!\n");
    }

    return EXIT_SUCCESS;
}