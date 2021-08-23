#include <stdio.h>
#include "sha256.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Invalid usage.\nUsage: ./sha256 {relative path to file}\n");
        return 1;
    }

    sha256(argv[1]);

    return 0;
}
