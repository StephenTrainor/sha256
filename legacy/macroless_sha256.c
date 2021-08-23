#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sha256.h"

static const uint32_t K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
 	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
   	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/*************************************************************************
 * Note: Function prototypes are only here so that I can keep 
 * sha256() at the top of the file. They are not necessary and 
 * can be removed as long as the function bodies are moved above sha256().
 ************************************************************************/

static inline uint32_t rotr(uint32_t x, uint8_t n);
static inline uint32_t shr(uint32_t x, uint8_t n);
static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t SIGMA0(uint32_t x);
static inline uint32_t SIGMA1(uint32_t x);
static inline uint32_t sigma0(uint32_t x);
static inline uint32_t sigma1(uint32_t x);
static char *getdelim_c(size_t *restrict n, int delim, FILE *restrict stream);
static bool little_endian(void);        // Function from https://www.cs-fundamentals.com/tech-interview/c/c-program-to-check-little-and-big-endian-architecture/

void sha256(char *restrict filename, uint32_t *restrict message_digest) {
	FILE* input_file = fopen(filename, "rb+");
	
	if (!input_file) { // Check for null pointer
		return; 
	} 
	
	size_t bytes = 0;

	char *M = getdelim_c(&bytes, '\0', input_file); // Read input_file into M

	if (bytes == -1) {
		bytes = 0; // Ensures that '\0' isn't hashed for an empty file
	}

	uint64_t l = bytes * 8;
	int N = ceil((64 + 1 + l) / BIT_BLOCK_SIZE) + 1;
	int k = (BIT_BLOCK_SIZE * N) - 65 - l;
	int i = 0;

	block *blocks = malloc(sizeof(block) * N);

	if (!blocks) {
		return;
	}

	uint8_t parts[8];

	if (little_endian()) {
		uint8_t temp_parts[8]; 

		memcpy(temp_parts, &l, sizeof(l));

		for (int i = 0; i < 8; i++) {
			parts[i] = temp_parts[7 - i]; // Reverse the array to convert to big-endian
		}
	}
	else {
		memcpy(parts, &l, sizeof(l)); // Already big-endian, no changes needed
	}

	bool finished_filling = false;
	bool finished_padding = false;

	int padding_bytes_needed = (k + 1) / 8;
	int bytes_filled = 0;
	int bytes_padded = 0;
	int t, x, y;

	for (x = 0; x < N; x++) {
		for (y = 0; y < BIT_BLOCK_SIZE / 8; y++) {
			if (bytes_filled == bytes) {
				finished_filling = true;
			}
			if (bytes_padded == padding_bytes_needed) {
                finished_padding = true;
			}

			if (finished_filling && !finished_padding) {
				if (bytes_padded != 0) {
					blocks[x].p[y] = 0x00; // Pad message with zeroes
				}
				else {
					blocks[x].p[y] = 0x80; // aka 0b10000000
				}
				bytes_padded++;
			}
			else if (finished_filling && finished_padding) {
				for (i = 0; i < 8; i++) {
					blocks[x].p[y + i] = parts[i]; // add the 64-bit message length signature
				}
				break;
			}
			else {
				blocks[x].p[y] = M[bytes_filled]; // Fill message block
				bytes_filled++;
			}
		}
	}

	uint32_t T1, T2; // Temp words

	uint32_t a, b, c, d, e, f, g, h; // Working variables

	uint32_t W[64]; // Message schedule

	uint32_t H[8] = { // Initial hash values
		0x6a09e667,
		0xbb67ae85,
		0x3c6ef372,
		0xa54ff53a,
		0x510e527f,
		0x9b05688c,
		0x1f83d9ab,
		0x5be0cd19
	};

	for (i = 0; i < N; i++) {
		for (t = 0; t < 64; t += 4) {
			W[t / 4] = blocks[i].p[t] << 24 | blocks[i].p[t + 1] << 16 | blocks[i].p[t + 2] << 8 | blocks[i].p[t + 3]; // Concat four uint8_t's into one 32-bit word
		}

		for (t = 16; t < 64; t++) { 
			W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16]; // Fill the message schedule
		}

		a = H[0]; // Initialize working variables
		b = H[1];
		c = H[2];
		d = H[3];
		e = H[4];
		f = H[5];
		g = H[6];
		h = H[7];

		for (t = 0; t < 64; t++) { // loop through message schedule and constants
			T1 = h + SIGMA1(e) + ch(e, f, g) + K[t] + W[t];
			T2 = SIGMA0(a) + maj(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		H[0] += a; // Calc the intermediate hash values
		H[1] += b;
		H[2] += c;
		H[3] += d;
		H[4] += e;
		H[5] += f;
		H[6] += g;
		H[7] += h;
	}

	for (i = 0; i < 8; i++) {
		message_digest[i] = H[i];
	}

	fclose(input_file);
	free(blocks);
	free(M);
}

static inline uint32_t rotr(uint32_t x, uint8_t n) {
	return (x >> n) | (x << (32 - n));
}

static inline uint32_t shr(uint32_t x, uint8_t n) {
	return x >> n;
}

static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
	return (x & y) ^ ((~x) & z);
}

static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
	return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint32_t SIGMA0(uint32_t x) {
	return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static inline uint32_t SIGMA1(uint32_t x) {
	return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static inline uint32_t sigma0(uint32_t x) {
	return rotr(x, 7) ^ rotr(x, 18) ^ shr(x, 3);
}

static inline uint32_t sigma1(uint32_t x) {
	return rotr(x, 17) ^ rotr(x, 19) ^ shr(x, 10);
}

static char *getdelim_c(size_t *restrict n, int delim, FILE *restrict stream) {
	if (!n) {
		return NULL;
	}

    if (!stream) { // Check for null stream
        (*n) = -1;
        return NULL;
    }

    char *buf = malloc(sizeof(char)); // space for terminator is allocated later
    char temp;

	if (!buf) {
		(*n) = -1;
		return NULL;
	}

    while ((temp = fgetc(stream)) != EOF && temp != delim) { // continue if char != specified delimeter
		if (temp != '\r') { // unwanted character that messes up message digest
			char *tmp_ptr = realloc(buf, (*n) + 1); // expand buffer

			if (!tmp_ptr) {
				free(buf);
				return NULL;
			}

			buf = tmp_ptr;
			buf[(*n)] = temp;

			(*n)++;
		}
    }

    char *tmp_ptr = realloc(buf, (*n) + 1);

    if (!tmp_ptr) {
        free(buf);
        return NULL;
    }

    buf = tmp_ptr;
    buf[(*n)] = '\0'; // Null terminate string

    return buf;
}

static bool little_endian(void) {
	unsigned int i = 1;
	char* c = (char*) &i;
	return (int)*c;
}
