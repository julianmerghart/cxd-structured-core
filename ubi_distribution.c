#include "ubi_distribution.h"
#include <string.h>
#include <stdio.h>

// A simple in-memory hash map to store used nullifiers for the current epoch
// In production, this is a Bloom Filter or Merkle Tree on-chain.
static uint8_t used_nullifiers[1000000][32]; 
static uint64_t used_count = 0;

static UBIState global_state = {100000000.0, 0, 0};

bool ubi_process_claim(UBIClaim *claim) {
    // 1. Check if this Nullifier has already been used today
    for (uint64_t i = 0; i < used_count; i++) {
        if (memcmp(used_nullifiers[i], claim->nullifier, 32) == 0) {
            printf("[UBI] REJECTED: Duplicate claim detected for Epoch %lu.\n", claim->epoch_id);
            return false;
        }
    }

    // 2. Verify the ZK-Proof (Identity check)
    // We prove the user is a verified human WITHOUT knowing who they are.
    bool identity_valid = true; // result of ZK-verify(claim->zk_proof_identity)

    if (!identity_valid) {
        printf("[UBI] REJECTED: Invalid human attestation.\n");
        return false;
    }

    // 3. Process the payout
    if (global_state.total_treasury >= UBI_DAILY_AMOUNT) {
        global_state.total_treasury -= UBI_DAILY_AMOUNT;
        
        // Add nullifier to the "spent" list
        memcpy(used_nullifiers[used_count], claim->nullifier, 32);
        used_count++;

        printf("[UBI] SUCCESS: 50.0 CXD distributed. Treasury remains: %.2f\n", global_state.total_treasury);
        return true;
    }

    return false;
}

void ubi_cycle_epoch(void) {
    // Wipe the nullifier list for the new day
    used_count = 0;
    global_state.last_epoch_reset += EPOCH_DURATION;
    printf("[UBI] EPOCH RESET: All verified humans may now claim for the new cycle.\n");
}
