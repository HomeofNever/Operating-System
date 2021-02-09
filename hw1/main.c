#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WORD_BUFFER 128
#define VALID_WORD_LENGTH 3

/** 
 * hash based on the ascii num
 * at this moment, word has no \0 ends and that's why we need size
 **/ 
int hash(const char *word, int cache_size, int word_size) {
    int sum = 0;
    for (int i = 0; i < word_size; i++) {
        sum += *(word + i);
    }
    return sum % cache_size;
}

/**
 * this function take the cache and word_buffer,
 * correctly set the end char and save to cache.
 **/
void set(char *word_buffer, int hash_num, int index, char **cache) {
    // end of word
    *(word_buffer + index) = '\0';
    // if current space is empty, we put in directly
    // or we reallocate the cache slot for our new word
    if (*(cache + hash_num) == NULL) {
        *(cache + hash_num) = calloc(sizeof(char), index + 1);
        printf("Word \"%s\" ==> %d (calloc)\n", word_buffer, hash_num);
    } else {
        *(cache + hash_num) = realloc(*(cache + hash_num), index + 1);
        printf("Word \"%s\" ==> %d (realloc)\n", word_buffer, hash_num);
    }

    // put word
    strcpy(*(cache + hash_num), word_buffer);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Error: missing required args. Usage: program <CACHE_SIZE> <FILE1> ...");
        return EXIT_FAILURE;
    }

    char *cache_size_char = *(argv + 1);
    int cache_size = strtol(cache_size_char, NULL, 10);
    if (cache_size <= 0) {
        // 0 will be returned if conversion has failed
        // but in this case, 0 is also not a valid cache_size
        fprintf(stderr, "Error: invalid cache size entered. %s", cache_size_char);
        return EXIT_FAILURE;
    }
    char **cache = calloc(sizeof(char *), cache_size);
    char *word_buffer = calloc(sizeof(char), WORD_BUFFER);

    // file args should start from 2
    int num_files = 2;
    while (num_files < argc) {
        int fd = open(*(argv + num_files), O_RDONLY);

        if (fd == -1) {
            perror("Error");
            return EXIT_FAILURE;
        }

        char letter;
        int index = 0;
        while (read(fd, &letter, 1) > 0) {
            // words will only contains letters and should longer than 3
            if ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z')) {
                *(word_buffer + index) = letter;
                index++;
            } else if (index != 0) {
                if (index >= VALID_WORD_LENGTH) {
                    set(word_buffer, hash(word_buffer, cache_size, index), index, cache);
                }

                // reset for next round
                // word_buffer will be override and is on stack
                // so we don't need to reset it
                index = 0;
            }
        }

        // we may have no separator at last
        if (index >= VALID_WORD_LENGTH) {
            set(word_buffer, hash(word_buffer, cache_size, index), index, cache);
        }

        close(fd);
        num_files++;
    }

    // print cache
    for (int i = 0; i < cache_size; i++) {
        if (*(cache + i) != NULL) {
            printf("Cache index %d ==> \"%s\"\n", i, *(cache + i));
        }
    }

    // clean up
    free(word_buffer);
    for (int i = 0; i < cache_size; i++) {
        if (*(cache + i) != NULL) {
            free(*(cache + i));
        }
    }
    free(cache);

    return EXIT_SUCCESS;
}
