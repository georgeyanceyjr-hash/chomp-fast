# V7 prerelease timing and validation

A fresh 18×18 run completed in **111.843082 seconds (1m 52s)** and produced a checkpoint byte-for-byte identical to validated V5/V6. This covers **37,993,832 unique P-positions**, including the original seed and dimension-18 base.

Compared with the earlier V6 run of **173.807009 seconds (2m 54s)**, this is **1.554× as fast, or 35.65% less time**. Both used the same fresh inputs, dimension 18 and 9 workers. The V6 time is from the earlier validation, not a newly repeated paired run. Wall time includes launch, loading, solving, checkpoint writing and exit, and excludes final hash checking.

| Test | V6 | V7 | Result |
|---|---:|---:|---|
| Fresh 16×16 | 10.77–11.10 s, two runs | 6.27 s | Complete checkpoint identical |
| Class R(17,16), same preload | 16.71 s | 11.13 s | Complete checkpoint identical |
| Fresh 18×18 | 173.81 s, earlier run | 111.84 s | All 120 records identical |

These measurements are local single-machine results; 19×19 speed is not yet measured for V7. V7 is a prerelease validated through 18×18 only. V6 remains the full 19×19 validated version. No 20×20 computation was performed.

## Changes retained

- Reduce batches from 262,144 to 16,384 candidates, making newly found losing positions available to the next parallel pass sooner.
- After parallel filtering misses, skip sequential corner lookups below the first candidate in that batch. Those old entries were already tested against an immutable database. New entries in the current class are in ascending order and cannot be below this bound; conjugates belong to the swapped class.
- Compute normalized boundary moves directly, eliminating temporary arrays, the general move transform and repeated normalization.
- Remove the preliminary lookup before insertion, retaining the exact set's duplicate check. Reuse the same hash for the Bloom filter and exact lookup.

A persistent thread pool was tried but was slightly slower in both combined comparisons (6.33 s for fresh 16×16 and 11.37 s for R(17,16)), so the selected candidate retains V6's joined worker threads. Isolated experimental builds are recorded separately; no isolated speed claim is made for hash reuse. Common compile flags for the V6 batch baselines and combined candidates were `-O3 -march=native -DNDEBUG -pthread -std=c++17`, Apple clang 17.0.0.

## Correctness evidence

The final source passes all 322 independently computed 8×8 P-positions and produces the identical 8×8 checkpoint. Boundary formulas passed 8,912,898 exhaustive comparisons with the old transform and 1,497,756 independent row-model comparisons through dimensions 31. A combined pool stress variant passed ASan/UBSan with 32-item batches and 1, 9 and 64 workers; the selected joined-thread version's barrier was independently reviewed.

The fresh 18×18 checkpoint ends at R(18,17), has 120 records and 37,993,702 stored entries, and is identical to the fully checked V5/V6 reference. Adding 28 base entries and 102 seed entries gives 37,993,832 unique positions. Identity with the reference also preserves its verified record checksums, ordering, class membership and bounds.

Checkpoint SHA-256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`

Candidate source SHA-256: `352af0da067d07ecc8816362f56f75b92d6fb8ff1d3264dcee1e007c1cb36466`

## Reproduce the fresh 18×18 validation

From the extracted source package, use the included original `seed.bin` and `base18.bin`. The output directory must be new, ensuring a fresh run. The runner verifies input hashes and rejects a different final checkpoint.

```sh
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v7_candidate.cpp -o chomp_sieve_v7_candidate
python3 validate_v7_18.py --solver ./chomp_sieve_v7_candidate --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
c++ -O3 -pthread -std=c++17 test_boundary_v7.cpp -o test_boundary_v7
./test_boundary_v7
```

Original seed and full 19×19 input assets remain available in the [V5 release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v5-19x19). Their results were corrected and validated in V5; earlier V4 results are not asserted valid here. V7 has not yet earned full 19×19 validation.
