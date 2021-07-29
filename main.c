#include <stdio.h>
#include "sha256.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid usage.\nUsage: ./sha256 {relative path to file}\n");
        return 1;
    }

    uint32_t H[8];

    sha256(argv[1], H);

    for (int i = 0; i < 8; i++) {
        printf("%08x", H[i]);
    }
    printf("\n");

    return 0;
}
