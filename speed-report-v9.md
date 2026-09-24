# V9 timing and validation report

Fresh 18×18 took **66.420068 seconds**, compared with **73.355474 seconds** for V8 run immediately beforehand: **9.45% less time** (1.104× as fast).
Both runs used the original bootstrap seed, dimension-18 base, the same machine and compiler flags, and 9 workers. These are measured single runs, not an average or confidence interval. Timing includes loading, solving, checkpoint writing and exit; it excludes compilation and the final hash comparison.

Both complete checkpoints match the validated V5/V6/V7/V8 18×18 checkpoint exactly. There are 120 records ending at R(18,17), 37,993,702 checkpoint positions, 102 seed positions and 28 base positions: **37,993,832 unique P-positions** in total.

V9 has been validated through 18×18 only. V8's previously measured full 19×19 time remains 416.457865 seconds; V9's full 19×19 runtime has not been measured. No 20×20 computation was started. This is the V9 prerelease, validated through 18×18 only.

## Retained changes

1. Use a cheaper exact-table hash (one multiplication and two xor-shifts). The Bloom prefilter and exact table consistently share it. Every possible filter match still receives an exact lookup.
2. Track the smallest and largest stored position in each class and immediately reject lookups outside that range. Bounds update on every successful insertion, including preload; rebuilding a table preserves them.
3. Generate the surviving candidate stream directly for classes wholly above the seed frontier: jump the top pointed-position interval, skip bottom pointed positions, and exclude the first skeleton and final rectangle. These are precisely the cases already rejected by V8's basic sieve. Candidate order, batch boundaries, counters and game decisions remain unchanged. The original enumeration remains for classes overlapping the seed frontier.

## Measurements

| Workload | V8 | Selected V9 logic | Output |
|---|---:|---:|---|
| Fresh 16×16 | 3.779 s | 3.308 s | Identical |
| R(17,16), same preload | 7.429 s | 6.748 s | Identical |
| Fresh 18×18 | 73.355 s | 66.420 s | Identical |

The smaller-workload variant name is `hash_range_generation`. Final V9 adds only the correct version label and an explicit successful return from main. Its final exact source and binary hashes are recorded by the full 18×18 runner.

Discarded experiments included alternative move orders, extra child-position sieves, a per-batch lookup table, tighter batch floors, three Bloom bits, a direct seed bitset, boundary caching, and using the coordinator as a worker. They did not outperform the selected combination enough to retain. All measured variants produced exact checkpoint matches; the full results are in `benchmarks.json`.

## Validation

- Exhaustive generation comparison: 848 parameter cases, 12,028,876 emitted candidates in exactly the original order, with identical counters; frontiers and batch sizes varied.
- Membership checks: 10,000 inserted keys, duplicate insertion, growth/rebuilding, and 200,000 exact negative queries.
- Corner checks: 3,344,280 independent row-model interior moves and 300,000 corner/floor membership decisions.
- Sanitizer checks: address and undefined-behavior sanitizers, 257-item stress batches, 1/9/17 workers, all reproducing the independent 322-position 8×8 reference checkpoint.
- Full 18×18 checkpoint SHA256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`.
- Final source SHA256: `6b42adb8956f1ee107ea80b4ddcf38428fc20de389894deb7c1635f67cb7b8ab`.

Matching the earlier complete checkpoint confirms preserved results over the stated range; it is not an independent mathematical proof of the inherited sieve.

## Reproduce

From this directory, with C++17 and Python 3:

```sh
clang++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v9_candidate.cpp -o chomp_sieve_v9_candidate
python3 validate_v9_18.py --solver ./chomp_sieve_v9_candidate --source chomp_sieve_v9_candidate.cpp --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
python3 check_generation_v9.py
clang++ -O2 -pthread -std=c++17 test_generation_v9.cpp -o test_generation
./test_generation
clang++ -O2 -pthread -std=c++17 test_membership_v9.cpp -o test_membership
./test_membership
clang++ -O2 -pthread -std=c++17 test_corner_v9.cpp -o test_corner
./test_corner seed.bin
```

The run output directory must be new. The runner verifies both input hashes and the full checkpoint hash. `reference18/` and `validation18/` bundle this comparison's logs and timing records. The generated checkpoints remain preserved locally and are reproduced by the runner; they are not duplicated in this compact package.
