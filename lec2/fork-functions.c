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
        return letter;
    }

    fprintf(stderr, "Error: failed read 6th character from data.txt\n");
    close(fd);
    abort();
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
        printf("PARENT: child process reported '%c'\n", (status >> (8)) & 0xff);
    }

    return EXIT_SUCCESS;
}