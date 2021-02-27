#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/stat.h>

#define VALID_WORD_LENGTH 1

/**
 * Sentinel execl()
 * @param writePipe pipe write fd
 */
void sentinel(int writePipe) {
    printf("CHILD: Calling execl() to execute sentinel.out...\n");
    int len = 8;
    char * fileName = calloc(len + 1, sizeof(char)); // \0
    strcpy(fileName, "sentinel");
    char * writeP = calloc(32, sizeof(char));
    sprintf(writeP, "%d", writePipe);
    write(writePipe, &len, sizeof(int));
    write(writePipe, fileName, 8);
    free(fileName);
    fflush(stdout);
    execl("./sentinel.out", "./sentinel.out", writeP, (char*) NULL);
    perror("Error");
    free(writeP);
    abort();
}

/**
 * Get file bytes via filename
 * @param fileName name of file to read
 * @return Bytes of the given file, -1 if file is not accessible
 */
int getBytes(char *fileName) {
    struct stat fileStat;
    if (stat(fileName, &fileStat) < 0) {
        perror("Error");
        return -1;
    }

    return fileStat.st_size;
}

/**
 * File stats method
 * @param writePipe pipe write fd
 * @param fileName name of file to read
 */
void child(int writePipe, char *fileName) {
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

        if (letter >= '0' && letter <= '9') {
            digitCount++;
        }
    }

    close(fd);

    // If line does not end with newline, we add one to it
    if (letter != '\n') {
        newlineCount++;
    }

    // no new line if file is empty :)
    if (theBytes == 0) {
        newlineCount = 0;
    }

    // calculate filename length and send for buffer
    int fileNameLength = strlen(fileName);
    write(writePipe, &fileNameLength, sizeof(fileNameLength));
    write(writePipe, fileName, fileNameLength);
    write(writePipe, &theBytes, sizeof(theBytes));
    write(writePipe, &wordCount, sizeof(wordCount));
    write(writePipe, &newlineCount, sizeof(newlineCount));
    write(writePipe, &digitCount, sizeof(digitCount));
    close(writePipe);

    printf("CHILD: Done processing \"%s\"\n", fileName);
    exit(EXIT_SUCCESS);
}

/**
 * Print String with given format
 * @param fileName name of the file
 * @param theByte total byte of the file
 * @param wordCount total words in the file
 * @param newlineCount total newlines in the file, +1 if not ended with new line, 0 if file is empty
 * @param digitCount number of digits in the file
 */
void printStr(const char *fileName, int theByte, int wordCount, int newlineCount, int digitCount) {
    if (strcmp(fileName, "sentinel") == 0) {
        printf("PARENT: Sentinel -- %d byte%s %d word%s %d line%s %d digit%s",
               theByte,
               theByte == 1 ? "," : "s,",
               wordCount,
               wordCount == 1 ? "," : "s,",
               newlineCount,
               newlineCount == 1 ? "," : "s,",
               digitCount,
               digitCount == 1 ? "\n" : "s\n");
    } else {
        printf("PARENT: File \"%s\" -- %d byte%s %d word%s %d line%s %d digit%s",
               fileName,
               theByte,
               theByte == 1 ? "," : "s,",
               wordCount,
               wordCount == 1 ? "," : "s,",
               newlineCount,
               newlineCount == 1 ? "," : "s,",
               digitCount,
               digitCount == 1 ? "\n" : "s\n");
    }
}

/**
 * Parent receiving info from children
 * @param children number of children created
 * @param pipes [[pid, pipe read fd] * children]
 */
void parent(int children, int ** pipes) {
    int status;  /* exit status from each child process */
    for (int i = 0; i < children; i++) {
        /* wait until a child process exits */
        waitpid(**(pipes + i), &status, 0);   /* BLOCKING CALL */
        if (WIFSIGNALED(status)) {
            fprintf(stderr, "PARENT: Child exited abnormally.\n");
        } else {
            /* read data from the pipe */
            int pipe_read = *(*(pipes + i) + 1);
            int theByte, wordCount, newlineCount, digitCount, strLength;
            if (read(pipe_read, &strLength, sizeof(strLength)) > 0) {
                char *fileName = calloc(strLength + 1, sizeof(char));
                if (read(pipe_read, fileName, strLength) > 0
                    && read(pipe_read, &theByte, sizeof(theByte)) > 0
                    && read(pipe_read, &wordCount, sizeof(wordCount)) > 0
                    && read(pipe_read, &newlineCount, sizeof(newlineCount)) > 0
                    && read(pipe_read, &digitCount, sizeof(digitCount)) > 0) {
                    *(fileName + strLength) = '\0';
                    printStr(fileName, theByte, wordCount, newlineCount, digitCount);
                    close(pipe_read);
                    free(fileName);
                } else {
                    fprintf(stderr, "PARENT: Child failed to read 4 data\n");
                }
            } else {
                fprintf(stderr, "PARENT: Child failed to read filename\n");
            }
        }
        free(*(pipes + i));
    }
    free(pipes);
}

int main(int argc, char **argv) {
    int numOfFiles = argc - 1;
    printf("PARENT: Collecting counts for %d file%sand the sentinel...\n",
           numOfFiles,
           numOfFiles == 1 ? " " : "s ");
    int ** pipes = calloc(argc, sizeof(int *));

    for (int i = 0; i < argc; i++) {
        int *p = calloc(2, sizeof(int));

        int pipe_rc = pipe(p);

        if (pipe_rc == -1) {
            perror("pipe() failed");
            return EXIT_FAILURE;
        }

        fflush(stdout);
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork() failed");
            return EXIT_FAILURE;
        }

        if (pid == 0) {
            if (i == argc - 1) {
                sentinel(*(p + 1));
            } else {
                child(*(p + 1), *(argv + i + 1));
            }
        } else {
            *(pipes + i) = calloc(2, sizeof(int));
            **(pipes + i) = pid;
            *(*(pipes + i) + 1) = *p;
            if (i == argc - 1) {
                printf("PARENT: Calling fork() to create child process for the sentinel\n");
            } else {
                printf("PARENT: Calling fork() to create child process for \"%s\"\n", *(argv + i + 1));
            }
            free(p);
        }
    }

    parent(argc, pipes);
    printf("PARENT: All done -- exiting...\n");

    return EXIT_SUCCESS;
}