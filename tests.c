#include <stdio.h>
#include "sha256.h"

char *files[] = {
    "texts/abc.txt", 
    "texts/abcabc.txt", 
    "texts/bible.txt", 
    "texts/characters.txt", 
    "texts/large.txt", 
    "texts/lorem.txt", 
    "texts/nothing.txt", 
    "texts/test.txt"
};

int main(void) {
    uint32_t H[8];

    for (unsigned int i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        sha256(files[i], H);

        printf("%s:\n", files[i]);
        for (int j = 0; j < 8; j++) {
            printf("%08x", H[j]);
        }
        printf("\n\n");

    }

    return 0;
}