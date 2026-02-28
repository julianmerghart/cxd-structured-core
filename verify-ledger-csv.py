import struct
import hashlib
import sys
import csv
import os
from datetime import datetime

# Binary format must match the C struct exactly
BLOCK_FORMAT = "<IIBBQ32s64s"
BLOCK_SIZE = struct.calcsize(BLOCK_FORMAT)

def verify_and_export(filename):
    if not os.path.exists(filename):
        print(f"Error: {filename} not found.")
        return

    csv_filename = filename.replace(".cxd", "_research_data.csv")
    
    with open(filename, "rb") as f, open(csv_filename, "w", newline='') as csvfile:
        # Prepare the CSV Header
        fieldnames = ['block_num', 'ref_id', 'total_votes', 'threshold_pct', 'outcome', 'timestamp_utc', 'status']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        # 1. Read Genesis
        data = f.read(BLOCK_SIZE)
        if not data: return
        
        prev_block_data = data
        block_count = 0
        
        print(f"--- Verifying & Exporting: {filename} ---")

        while True:
            # Unpack current block for CSV recording
            curr = struct.unpack(BLOCK_FORMAT, prev_block_data)
            
            # Format data for human readability
            readable_time = datetime.utcfromtimestamp(curr[4]).strftime('%Y-%m-%d %H:%M:%S')
            outcome_str = "PASS" if curr[3] == 1 else "FAIL"
            
            writer.writerow({
                'block_num': block_count,
                'ref_id': curr[0],
                'total_votes': curr[1],
                'threshold_pct': curr[2],
                'outcome': curr[3],
                'timestamp_utc': readable_time,
                'status': "Final Result" if curr[5] != b'\x00'*32 else "Heartbeat"
            })

            # 2. Look ahead for next block to verify hash
            next_data = f.read(BLOCK_SIZE)
            if not next_data: break
            
            # Verify the cryptographic link
            hasher = hashlib.sha256()
            hasher.update(prev_block_data)
            if hasher.digest() != struct.unpack(BLOCK_FORMAT, next_data)[5]:
                print(f"❌ TAMPERING DETECTED at block {block_count + 1}!")
                return

            prev_block_data = next_data
            block_count += 1

        print(f"✅ Success. Research data exported to: {csv_filename}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python verify_and_export.py <ledger_file.cxd>")
    else:
        verify_and_export(sys.argv[1])
