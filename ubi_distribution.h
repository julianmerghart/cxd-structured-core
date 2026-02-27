#ifndef UBI_DISTRIBUTION_H
#define UBI_DISTRIBUTION_H

#include <stdint.h>
#include <stdbool.h>

#define UBI_DAILY_AMOUNT 50.0 // 50 CXD per day
#define EPOCH_DURATION 86400  // Seconds in a day

typedef struct {
    uint64_t epoch_id;         // Current day/cycle
    uint8_t nullifier[32];     // ZK-Nullifier (prevents double-claim)
    uint8_t zk_proof_identity[256]; // Proof that user is a verified human
} UBIClaim;

typedef struct {
    double total_treasury;
    uint64_t total_verified_users;
    uint64_t last_epoch_reset;
} UBIState;

/**
 * @brief Verifies a UBI claim and processes the payout.
 * @param claim The user's anonymous claim packet.
 * @return true if payout is processed, false if nullifier is already used or identity is invalid.
 */
bool ubi_process_claim(UBIClaim *claim);

/**
 * @brief Resets the nullifier blacklist for a new epoch (day).
 */
void ubi_cycle_epoch(void);

#endif // UBI_DISTRIBUTION_H
