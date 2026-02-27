#ifndef VETTING_GATE_H
#define VETTING_GATE_H

#include <stdint.h>
#include <stdbool.h>

// Status codes for the vetting process
typedef enum {
    PROPOSAL_PENDING,
    PROPOSAL_VETTING,
    PROPOSAL_ACTIVE,
    PROPOSAL_REJECTED,
    PROPOSAL_SPAM_BURNED
} ProposalStatus;

typedef struct {
    uint64_t proposal_id;
    char title[128];
    double staked_cxd;
    ProposalStatus status;
} Referendum;

typedef struct {
    uint64_t user_id;
    double balance_cxd;
} CxDUser;

/**
 * @brief Fetches the current gold spot price and CXD market price from the Oracle.
 * @return The required vetting fee in CXD (equivalent to 1 troy oz of Gold).
 */
double vetting_gate_calculate_required_fee(void);

/**
 * @brief Attempts to submit a referendum by locking the gold-pegged fee.
 * @return true if submission is successful, false if insufficient funds.
 */
bool vetting_gate_submit_proposal(Referendum *ref, CxDUser *author);

/**
 * @brief Handles the burning of the stake if the Security Council flags malicious intent.
 */
void vetting_gate_slash_stake(Referendum *ref);

#endif // VETTING_GATE_H
