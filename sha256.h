#ifndef SHA256_H
#define SHA256_H

#define BIT_BLOCK_SIZE 512
#define BLOCK_BYTES 64

#include <stdint.h>

typedef struct {
	uint8_t p[BLOCK_BYTES];
} block;

void sha256(char *restrict filename);

#endif

