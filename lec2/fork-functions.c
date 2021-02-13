#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int lecex2_child()
{
    int fd = open("data.txt", O_RDONLY);

    if (fd == -1)
    {
        perror("Error");
        abort();
    }

    char letter;
    if (lseek(fd, 5, SEEK_CUR) > 0 && read(fd, &letter, 1) > 0)
    {
        close(fd);
        exit(letter);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Error: failed to read 6th character from data.txt\n");
    close(fd);
    abort();

    return EXIT_FAILURE;
}

int lecex2_parent()
{
    int status;
    waitpid(0, &status, 0); // Blocking;

    if (WIFSIGNALED(status))
    {
        printf("PARENT: child process terminated abnormally\n");
        return EXIT_FAILURE;
    }
    else
    {
        printf("PARENT: child process reported '%c'\n", WEXITSTATUS(status));
    }

    return EXIT_SUCCESS;
}