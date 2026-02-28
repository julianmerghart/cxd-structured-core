#include "ledger.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Note: In a real deployment, you would link a tiny SHA-256 lib here.
// For this example, we'll assume a 'calc_sha256' helper exists.
extern void calc_sha256(uint8_t *out, const void *data, size_t len);

static void get_ledger_path(uint32_t ref_id, char *path) {
    sprintf(path, "ref_%u.cxd", ref_id);
}

int create_referendum_ledger(uint32_t ref_id) {
    char path[64];
    get_ledger_path(ref_id, path);
    
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    
    // Write a "Genesis Heartbeat" to start the chain
    CxDBlock genesis = { .ref_id = ref_id, .timestamp = (uint64_t)time(NULL) };
    memset(genesis.prev_hash, 0, 32); // No previous hash for genesis
    
    fwrite(&genesis, sizeof(CxDBlock), 1, f);
    fclose(f);
    return 0;
}

int seal_referendum(uint32_t ref_id, uint32_t total, uint8_t threshold, uint8_t outcome) {
    char path[64];
    get_ledger_path(ref_id, path);
    
    // 1. Read the last heartbeat to get the hash
    FILE *f = fopen(path, "rb+");
    if (!f) return -1;
    
    fseek(f, -sizeof(CxDBlock), SEEK_END);
    CxDBlock last_block;
    fread(&last_block, sizeof(CxDBlock), 1, f);
    
    // 2. Prepare the Final Seal Block
    CxDBlock final_block;
    final_block.ref_id = ref_id;
    final_block.total_votes = total;
    final_block.threshold_pct = threshold;
    final_block.outcome = outcome;
    final_block.timestamp = (uint64_t)time(NULL);
    
    // Create the immutable link
    calc_sha256(final_block.prev_hash, &last_block, sizeof(CxDBlock));
    
    // 3. Append and Flush (Instant Reveal)
    fseek(f, 0, SEEK_END);
    fwrite(&final_block, sizeof(CxDBlock), 1, f);
    
    // Secure deployment: force the OS to write to physical disk immediately
    fflush(f);
#ifdef _WIN32
    _commit(_fileno(f));
#else
    fsync(fileno(f));
#endif
    
    fclose(f);
    return 0;
}
