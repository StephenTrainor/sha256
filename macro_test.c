#include <stdio.h>
#include "macro.h"

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
    // if (argc != 2) {
    //     printf("Invalid usage.\nUsage: ./sha256 {relative path to file}\n");
    //     return 1;
    // }

    for (int i = 0; i < 8; i++) {
        sha256(files[i]);
    }

    return 0;
}