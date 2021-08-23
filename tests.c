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
    int i;
    for (i = 0; i < 8; i++) {
        sha256(files[i]);
    }

    return 0;
}