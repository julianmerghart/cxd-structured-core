#include "golden-gate.h"
#include <stdio.h>

// Simulated external Oracle values
// In production, these would be calls to a Trusted Execution Environment (TEE)
static double mock_fetch_gold_price() { return 2105.50; } // USD per oz
static double mock_fetch_cxd_price() { return 5.25; }    // USD per CXD

double vetting_gate_calculate_required_fee(void) {
    double gold_price = mock_fetch_gold_price();
    double cxd_price = mock_fetch_cxd_price();
    
    if (cxd_price <= 0) return 0;
    
    return gold_price / cxd_price;
}

bool vetting_gate_submit_proposal(Referendum *ref, CxDUser *author) {
    double required_fee = vetting_gate_calculate_required_fee();

    if (author->balance_cxd < required_fee) {
        fprintf(stderr, "[VettingGate] ERROR: Insufficient stake. Need %.2f CXD.\n", required_fee);
        return false;
    }

    // Process the lock
    author->balance_cxd -= required_fee;
    ref->staked_cxd = required_fee;
    ref->status = PROPOSAL_VETTING;

    printf("[VettingGate] SUCCESS: Proposal %lu submitted with %.2f CXD stake.\n", 
            ref->proposal_id, ref->staked_cxd);
            
    return true;
}

void vetting_gate_slash_stake(Referendum *ref) {
    if (ref->status == PROPOSAL_VETTING || ref->status == PROPOSAL_ACTIVE) {
        printf("[VettingGate] ALERT: Malicious intent detected. Burning %.2f CXD.\n", 
                ref->staked_cxd);
        
        ref->staked_cxd = 0;
        ref->status = PROPOSAL_SPAM_BURNED;
        
        // Internal protocol call to permanently remove tokens from total supply
        // burn_tokens_permanently(ref->staked_cxd);
    }
}
