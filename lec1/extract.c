#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: extract [file]");
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        printf("Unable to read file %s", argv[1]);
        return EXIT_FAILURE;
    }

    char letter;
    while (lseek(fd, 6, SEEK_CUR) > 0 && read(fd, &letter, 1) > 0) {
            printf("%c", letter);
    }

    printf("\n");
    close(fd);

    return EXIT_SUCCESS;
}