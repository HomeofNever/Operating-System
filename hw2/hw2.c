#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/stat.h>

#define VALID_WORD_LENGTH 1

int sentinel() {
    return 0;
}

int getBytes(char *fileName) {
    struct stat fileStat;
    if (stat(fileName, &fileStat) < 0) {
        perror("Error");
        return -1;
    }

    return fileStat.st_size;
}

void child(int * thePipe, char *fileName) {
    printf("CHILD: Processing \"%s\"\n", fileName);
    int theBytes = getBytes(fileName);
    // Check if file exist
    if (theBytes < 0) {
        abort();
    }

    int fd = open(fileName, O_RDONLY);

    if (fd == -1) {
        perror("Error");
        abort();
    }

    char letter;
    int wordCount = 0;
    int newlineCount = 0;
    int digitCount = 0;
    int index = 0;
    while (read(fd, &letter, 1) > 0) {
        // words will only contains letters
        if ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z')) {
            index++;
        } else if (index != 0) {
            if (index >= VALID_WORD_LENGTH) {
                wordCount++;
            }
            // reset for next round
            index = 0;
        }

        if (letter == '\n') {
            newlineCount++;
        }

        if (letter > '0' && letter < '9') {
            digitCount++;
        }
    }

    close(fd);

    // If line does not end with newline, we add one to it
    if (letter != '\n') {
        newlineCount++;
    }

    // calculate filename length and send for buffer
    int fileNameLength = strlen(fileName);
    write(*(thePipe + 1), &theBytes, sizeof(theBytes));
    write(*(thePipe + 1), &wordCount, sizeof(wordCount));
    write(*(thePipe + 1), &newlineCount, sizeof(newlineCount));
    write(*(thePipe + 1), &digitCount, sizeof(digitCount));
    write(*(thePipe + 1), &fileNameLength, sizeof(fileNameLength));
    write(*(thePipe + 1), fileName, fileNameLength);
    close(*(thePipe + 1));

    printf("CHILD: Done processing \"%s\"\n", fileName);
    int fd_read = *thePipe;
    // Free fd array before exit
    free(thePipe);
    exit(fd_read);
}


void parent(int children) {
    int status;  /* exit status from each child process */

    while ( children > 0 )
    {
        /* wait until a child process exits */
        waitpid( -1, &status, 0 );   /* BLOCKING CALL */

        children--;

        /* read data from the pipe */
        if (WIFSIGNALED(status)) {
            fprintf(stderr, "PARENT: Child exited abnormally.\n");
        } else {
            int pipe_read = WEXITSTATUS(status);
            int theByte, wordCount, newlineCount, digitCount, strLength;
            if (read(pipe_read, &theByte, sizeof(theByte)) > 0
                && read(pipe_read, &wordCount, sizeof(wordCount)) > 0
                && read(pipe_read, &newlineCount, sizeof(newlineCount)) > 0
                && read(pipe_read, &digitCount, sizeof(digitCount)) > 0
                && read(pipe_read, &strLength, sizeof(strLength)) > 0 ) {
                char * fileName = calloc(strLength + 1, sizeof(char));
                read(pipe_read, fileName, strLength);
                *(fileName + strLength) = '\0';
                printf("PARENT: File \"%s\" -- %d bytes, %d words, %d lines, %d digits\n",
                       fileName, theByte, wordCount, newlineCount, digitCount);
                close(pipe_read);
                free(fileName);
            } else {
                fprintf(stderr, "PARENT: Child failed to read some data\n");
            }
        }
    }
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0); // for submitty ONLY
    int numOfFiles = argc - 1;
    printf("PARENT: Collecting counts for %d file and the sentinel...\n", numOfFiles);

    for (int i = 1; i < argc; i++) {
        int * p = calloc(2, sizeof(int));

        int pipe_rc = pipe(p);

        if (pipe_rc == -1) {
            perror("pipe() failed");
            return EXIT_FAILURE;
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork() failed");
            return EXIT_FAILURE;
        }

        if (pid == 0) {
            child(p, *(argv + i));
        } else {
            printf("PARENT: Calling fork() to create child process for \"%s\"\n", *(argv + i));
        }
    }

    parent(argc - 1); // @TODO sentinel
    printf("PARENT: All done -- exiting...\n");

    return EXIT_SUCCESS;
}