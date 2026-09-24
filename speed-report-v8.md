# V8 timing and validation report

The fresh 18×18 computation completed in **74.092061 seconds**, with all 120 checkpoint records byte-for-byte identical to V5, V6 and V7. Including the unchanged seed and base cases, the result contains **37,993,832 unique P-positions**.

Compared with V7's earlier 111.843082-second run, V8 uses **33.75% less time** (1.510× as fast). Both used the same bootstrap inputs and 9 workers. These are single-machine runs, not an average or confidence interval. The current V7 reference was also timed on the smaller comparison cases below. Wall time includes launch, loading, solving, checkpoint writing and exit; hashing is excluded.

| Workload | V7 | Selected V8 logic | Checkpoint |
|---|---:|---:|---|
| Fresh 16×16 | 6.189 s | 3.795–3.813 s, two runs | Identical |
| R(17,16), same preload | 11.055 s | 7.073 s | Identical |
| Fresh 18×18 | 111.843 s, earlier validation | 74.092 s | Identical |

For context, V6's earlier fresh 18×18 run took 173.807009 seconds. V8 has only been validated through 18×18. No V8 19×19 or 20×20 computation has run, and V8 is published as an 18×18-only prerelease. V7 remains available; V6 remains the implementation validated through the complete 19×19 run.

## Changes retained

- Traverse corner moves directly with set-bit masks instead of building temporary arrays. Skip entire move groups whose first changed bit proves that all outcomes lie below the current batch floor.
- Check four Bloom-filter bits in one 64-bit word load. Every possible match still receives an exact hash-table lookup, so the filter introduces no approximate game decisions.
- Allocate parallel work dynamically in blocks of 128 candidates. Each block has one owner, the database is immutable while workers run, and every thread joins before the original ordered insertion pass.
- Replace Gosper's division by the lowest set bit with the equivalent trailing-zero count and shift.

The batch size remains 16,384 and the measured run uses 9 workers. Further batch-size changes, reduced reservations, structural pointed-position skips, and a bit-reversal conjugation trial did not improve the selected configuration enough to retain. The original scalar conjugation remains. The two historical benchmark rows named `v8_candidate` before the full run refer to the discarded combined conjugation trial; `combined_word4` is the retained logic. `build-and-inputs.json` and the full-run source hash identify the final candidate exactly.

## Correctness

All 322 independent 8×8 positions match, including the full checkpoint hash. The final corner implementation passed 300,000 membership/floor decisions against 3,344,280 independent row-model interior moves. The lookup fixture exercises 10,000 inserted keys, duplicate insertion, growth/rebuilding and 200,000 negative queries with exact reference membership. Division-to-shift equivalence was checked on 100,000 random successors through dimensions 31. ASan/UBSan scheduling stress uses a 257-item batch with 1, 9 and 17 workers and checks identical 8×8 checkpoints.

The fresh 18×18 checkpoint ends at R(18,17). Its exact identity with the fully checked V5/V6/V7 reference preserves that reference's valid checksums, order, dimensions and count. Agreement with this reference is not an independent mathematical proof of the inherited sieve.

Checkpoint SHA-256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`

Source SHA-256: `6360e13e0d7c08811e587ef5c8e0a6695ca0a28e25ff1183d35dd79a164c0d9a`

## Reproduce

From this directory (C++17 and Python 3), using the included original `seed.bin` and `base18.bin`:

```sh
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v8_candidate.cpp -o chomp_sieve_v8_candidate
python3 validate_v8_18.py --solver ./chomp_sieve_v8_candidate --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
c++ -O3 -pthread -std=c++17 test_corner_v8.cpp -o test_corner_v8
./test_corner_v8 seed.bin
c++ -O3 -pthread -std=c++17 test_membership_v8.cpp -o test_membership_v8
./test_membership_v8
```

The fresh-run output directory must not already exist. The runner verifies input and checkpoint hashes. The recorded machine used Apple clang 17.0.0; optimization flags were `-O3 -march=native -DNDEBUG -pthread -std=c++17` throughout the measured variants.
