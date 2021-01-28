#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WORD_BUFFER 128

int hash(const char * word, int cache_size, int word_size) {
    int sum = 0;
    for (int i = 0; i <= word_size; i++) {
        sum += word[i];
    }

    return sum % cache_size;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Error: missing required args. Usage: program [CACHE_SIZE] [file1] [file2]...");
        return EXIT_FAILURE;
    }

    int cache_size = strtol(argv[1], NULL, 10);
    if (cache_size <= 0) {
        // 0 will return if conversion failed.
        // But in this case, 0 is also not a valid cache_size.
        fprintf(stderr, "Error: invalid cache size entered. %s", argv[1]);
        return EXIT_FAILURE;
    }
    char ** cache = calloc(sizeof(char *), cache_size);
    char * word_buffer = calloc(sizeof(char), WORD_BUFFER);

    int num_files = 2;
    while (num_files < argc) {
        int fd = open(argv[num_files], O_RDONLY);

        if (fd == -1) {
            perror("Error");
            return EXIT_FAILURE;
        }


        char letter;
        int index = 0;
        while (read(fd, &letter, 1) > 0) {
            if ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z')) {
                word_buffer[index] = letter;
                index++;
            } else if (index != 0) {
                // end of word
                word_buffer[index] = '\0';
                // make up space
                int hash_num = hash(word_buffer, cache_size, index);
                if (cache[hash_num] == NULL) {
                    cache[hash_num] = calloc(sizeof(char), index + 1);
                    printf("Word \"%s\" ==> %d (calloc)\n", word_buffer, index);
                } else {
                    cache[hash_num] = realloc(cache[hash_num], index + 1);
                    printf("Word \"%s\" ==> %d (realloc)\n", word_buffer, index);
                }

                // put word
                strcpy(cache[hash_num], word_buffer);

                // For next round
                index = 0;
            }
        }

        close(fd);
        num_files++;
    }

    for (int i = 0; i < cache_size; i++) {
        if (cache[i] != NULL) {
            printf("Cache index %d ==> \"%s\"\n", i, cache[i]);
        }
    }

    // clean up
    free(word_buffer);
    for (int i = 0; i < cache_size; i++) {
        if (cache[i] != NULL) {
            free(cache[i]);
        }
    }
    free(cache);

    return EXIT_SUCCESS;
}
