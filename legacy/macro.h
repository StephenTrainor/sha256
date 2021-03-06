#ifndef MACRO_SHA256_H
#define MACRO_SHA256_H

#define BIT_BLOCK_SIZE 512

#include <stdint.h>

typedef struct {
	uint8_t p[64];
} block;

void sha256(char *restrict filename);

#endif
