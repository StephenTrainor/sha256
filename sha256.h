#ifndef SHA256_H
#define SHA256_H

#define BIT_BLOCK_SIZE 512

#include <stdint.h>

typedef struct {
	uint8_t p[64];
} block;

void sha256(char *restrict filename, uint32_t *restrict message_digest);

#endif

