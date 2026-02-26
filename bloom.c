#include <stdio.h>
#include <string.h>
#include "cxd_core.h"

#define BLOOM_SIZE 1048576 // 1MB bit array for fast lookup

unsigned char global_bloom[BLOOM_SIZE];

// Simple hash for the Bloom filter
unsigned int bloom_hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash % (BLOOM_SIZE * 8);
}

int is_duplicate_vote(const char *voter_hash) {
    unsigned int bit_pos = bloom_hash(voter_hash);
    if (global_bloom[bit_pos / 8] & (1 << (bit_pos % 8))) {
        return 1; // Probable duplicate
    }
    // Set the bit to mark this hardware as "voted"
    global_bloom[bit_pos / 8] |= (1 << (bit_pos % 8));
    return 0;
}
