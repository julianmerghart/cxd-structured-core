#ifndef CXD_LEDGER_H
#define CXD_LEDGER_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint32_t ref_id;          // Unique Referendum ID
    uint32_t total_votes;     // Total participation (for researchers)
    uint8_t  threshold_pct;   // e.g., 75 for 75% requirement
    uint8_t  outcome;         // 1 = PASS, 0 = FAIL
    uint64_t timestamp;       // Unix epoch of finalization
    uint8_t  prev_hash[32];   // SHA-256 link to the previous 'Heartbeat' block
    uint8_t  block_sig[64];   // Hardware-bound signature (TPM/Secure Enclave)
} CxDBlock;
#pragma pack(pop)

// API for the Micro-Ledger
int create_referendum_ledger(uint32_t ref_id);
int record_heartbeat(uint32_t ref_id, uint32_t current_total);
int seal_referendum(uint32_t ref_id, uint32_t total, uint8_t threshold, uint8_t outcome);

#endif
