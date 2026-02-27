#ifndef RECURSIVE_SNARK_H
#define RECURSIVE_SNARK_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_BATCH_SIZE 1024
#define PROOF_SIZE 256 // Bytes

typedef struct {
    uint8_t raw_data[PROOF_SIZE];
} ZKProof;

typedef struct {
    ZKProof leaf_proofs[MAX_BATCH_SIZE];
    uint32_t active_count;
    uint256_t state_root; // Represents the current Merkle Root of the batch
} ProofBatch;

/**
 * @brief Aggregates two ZK-SNARK proofs into a single recursive proof.
 * @return A new ZKProof representing the validity of both inputs.
 */
ZKProof snark_compress_recursive(ZKProof p1, ZKProof p2);

/**
 * @brief Processes a full batch of proofs, reducing them to one Master Proof.
 */
ZKProof snark_generate_master_proof(ProofBatch *batch);

/**
 * @brief Final verification on the global ledger.
 */
bool snark_verify_master_on_chain(ZKProof master_proof, uint256_t expected_root);

#endif // RECURSIVE_SNARK_H
