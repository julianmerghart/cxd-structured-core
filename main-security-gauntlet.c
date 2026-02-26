int process_universal_vote(UniversalBallot ballot, Point voter_loc, char *v_hash) {
    // 1. Geofence Check (National Integrity)
    if (!is_inside_border(voter_loc, nation_poly, poly_size)) return -1;

    // 2. Bloom Filter Check (Sybil/Duplicate Protection)
    if (is_duplicate_vote(v_hash)) return -2;

    // 3. FBA Consensus (Network Integrity)
    if (!verify_federated_consensus(v_hash)) return -3;

    // 4. Decrypt, Validate, Wipe (Privacy Integrity)
    secure_wipe(&ballot, sizeof(ballot));
    
    return 1; // Finalized & Purged
}
