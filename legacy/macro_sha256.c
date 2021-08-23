#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "macro.h" // same as sha256.h

#define r(x, y) (((((unsigned long)(x) & 0xFFFFFFFFUL) >> (unsigned long) ((y) & 31)) | ((unsigned long)(x) << (unsigned long) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)
#define s(x, n) (((x) & 0xFFFFFFFFUL) >> (n))
#define ch(x, y, z) (z ^ (x & (y ^ z)))
#define maj(x, y, z) (((x | y) & z) | (x & y))
#define SIGMA0(x) (r(x, 2) ^ r(x, 13) ^ r(x, 22))
#define SIGMA1(x) (r(x, 6) ^ r(x, 11) ^ r(x, 25))
#define sigma0(x) (r(x, 7) ^ r(x, 18) ^ s(x, 3))
#define sigma1(x) (r(x, 17) ^ r(x, 19) ^ s(x, 10))

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

void sha256(char *restrict filename) {
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
    int i, x, y;

	block *blocks = malloc(sizeof(block) * N);

	if (!blocks) {
		return;
	}

	uint8_t parts[8];
	memcpy(parts, &l, sizeof(l));

	if (little_endian()) {
		for (i = 0; i < 8; i++) {
			blocks[N - 1].p[56 + i] = parts[7 - i]; // Reverse the array to convert to big-endian
		}
	}
    else {
        for (i = 0; i < 8; i++) {
            blocks[N - 1].p[56 + i] = parts[i];
        }
    }

	bool finished_filling = false;

	int padding_bytes_needed = (k + 1) / 8, bytes_filled = 0, bytes_padded = 0;

	for (x = 0; x < N; x++) {
		for (y = 0; y < BIT_BLOCK_SIZE / 8; y++) {
			if (bytes_filled == bytes) {
				finished_filling = true;
			}
			if (bytes_padded == padding_bytes_needed) {
                break;
			}

			if (finished_filling) {
				if (bytes_padded != 0) {
					blocks[x].p[y] = 0x00; // Pad message with zeroes
				}
				else {
					blocks[x].p[y] = 0x80; // aka 0b10000000
				}
				bytes_padded++;
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
        int t;
		for (t = 0; t < 64; t += 4) {
			W[t / 4] = blocks[i].p[t] << 24 | blocks[i].p[t + 1] << 16 | blocks[i].p[t + 2] << 8 | blocks[i].p[t + 3]; // Concat four uint8_t's into one 32-bit word
		}

		for (t = 16; t < 64; ++t) { 
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

        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x428a2f98 + W[0]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x71374491 + W[1]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xb5c0fbcf + W[2]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xe9b5dba5 + W[3]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x3956c25b + W[4]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x59f111f1 + W[5]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x923f82a4 + W[6]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xab1c5ed5 + W[7]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xd807aa98 + W[8]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x12835b01 + W[9]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x243185be + W[10]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x550c7dc3 + W[11]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x72be5d74 + W[12]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x80deb1fe + W[13]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x9bdc06a7 + W[14]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xc19bf174 + W[15]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xe49b69c1 + W[16]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xefbe4786 + W[17]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x0fc19dc6 + W[18]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x240ca1cc + W[19]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x2de92c6f + W[20]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x4a7484aa + W[21]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x5cb0a9dc + W[22]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x76f988da + W[23]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x983e5152 + W[24]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xa831c66d + W[25]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xb00327c8 + W[26]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xbf597fc7 + W[27]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xc6e00bf3 + W[28]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xd5a79147 + W[29]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x06ca6351 + W[30]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x14292967 + W[31]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x27b70a85 + W[32]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x2e1b2138 + W[33]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x4d2c6dfc + W[34]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x53380d13 + W[35]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x650a7354 + W[36]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x766a0abb + W[37]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x81c2c92e + W[38]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x92722c85 + W[39]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xa2bfe8a1 + W[40]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xa81a664b + W[41]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xc24b8b70 + W[42]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xc76c51a3 + W[43]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xd192e819 + W[44]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xd6990624 + W[45]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xf40e3585 + W[46]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x106aa070 + W[47]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x19a4c116 + W[48]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x1e376c08 + W[49]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x2748774c + W[50]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x34b0bcb5 + W[51]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x391c0cb3 + W[52]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x4ed8aa4a + W[53]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x5b9cca4f + W[54]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x682e6ff3 + W[55]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x748f82ee + W[56]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x78a5636f + W[57]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x84c87814 + W[58]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x8cc70208 + W[59]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0x90befffa + W[60]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xa4506ceb + W[61]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xbef9a3f7 + W[62]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;
        // T1 = h + SIGMA1(e) + ch(e, f, g) + 0xc67178f2 + W[63]; T2 = SIGMA0(a) + maj(a, b, c); h = g; g = f; f = e; e = d + T1; d = c; c = b; b = a; a = T1 + T2;

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
        printf("%08x", H[i]);
	}
    printf("\n");

	fclose(input_file);
	free(blocks);
	free(M);
}
