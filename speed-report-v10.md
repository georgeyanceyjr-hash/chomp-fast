# V10 timing and validation report

Across two fresh 18×18 runs per version, V10 averaged **64.743236 seconds** versus **66.133774 seconds** for V9: **1.390538 seconds saved, 2.10% less time** (1.021× as fast).

All runs used the same original bootstrap seed, dimension-18 base, compiler flags, machine and 9 workers. Runs were sequential in order V9, V10, V10, V9 and no other solver benchmark ran alongside them. Reversing the order provided a check on the small gain. This remains a limited sample, not a statistical confidence estimate. Timing includes input loading, solving, checkpoint writing and process exit; compilation and final hash checking are excluded.

All four complete 303,952,504-byte checkpoints match exactly, including all 120 records ending at R(18,17). This preserves all **37,993,832 unique P-positions**: 37,993,702 checkpoint entries plus 102 bootstrap and 28 base positions.

V10 is validated through 18×18. Its full 19×19 runtime has not been measured. The previous V9 19×19 run took 387.906746 seconds. This is the V10 prerelease, validated through18×18.

## Retained changes

- Stop checking corner moves once their increasing keys exceed the largest stored position in the class.
- Insert each class's transposed positions together at class completion. Those transposes cannot affect any move in the current class.
- Sort only transposed new positions and merge them with the already sorted originals when writing checkpoints.

The exact data format and class-boundary restart behavior remain compatible. `implementation-notes.md` explains the reasoning, including asymmetric preloads. Every filter match still receives an exact membership lookup.

## Measurements

| Workload | V9 | V10 selected logic | Result |
|---|---:|---:|---|
| Fresh 16×16 | 3.326305 s | 3.124915 s | Identical checkpoint |
| R(17,16), same preload | 6.744571 s | 6.527326 s | Identical checkpoint |
| Fresh 18×18, first pair (V9 then V10) | 65.808957 s | 64.636945 s | Identical checkpoint |
| Fresh 18×18, second pair (V10 then V9) | 66.458591 s | 64.849527 s | Identical checkpoint |
| Fresh 18×18, mean | 66.133774 s | 64.743236 s | Identical checkpoint |

The selected smaller-workload variant was `max_deferred_merged`. The timed 18×18 source changes only its version comment and is preserved as `chomp_sieve_v10_timed.cpp`. After timing, the final candidate received one further label-only edit: correcting the executable name printed when no arguments are supplied. `label-only-change.json` records and verifies that exact string replacement; solver logic is unchanged, and the final binary passed the 11-case preload/resume suite again. Both source and binary hashes are recorded separately in the manifest.

`benchmarks.json` preserves all trials. Alternatives tested included larger Bloom filters, separate Bloom bit lanes, SIMD mask generation, profile-guided compilation, specialized zero-floor lookups, cached zero-endpoint calculations and alternate batch storage. They did not produce a useful gain beyond this combination and were discarded.

## Correctness checks

- 3,344,280 independent row-model moves, all in strictly increasing query order, and 300,000 corner/floor decisions against both V9 and an older independently structured kernel.
- 65 appended checkpoint records covering 432,160 values: the merge writer reproduces the original writer byte for byte, including empty records and writes larger than its buffer.
- 11 preload/resume scenarios, 22 small solver invocations against V9: asymmetric data, missing transposes, selected-class execution, interrupted runs resumed at class boundaries and a completed no-op restart. Every resulting catalog matched the independent 322-position 8×8 fixture.
- Address/undefined-behavior sanitizer checks with 257-item batches and 1, 9 and 17 workers: all produced the independent 8×8 checkpoint with no findings.
- Fresh complete 18×18 checkpoint SHA256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`.
- V10 source SHA256: `2e0ce1430a1e5d59cd424c50504687aea693ed11f157fd1f5dbaad7cb2b725b7`.
- Timed V10 source SHA256: `6a0d59c02e7d680988d4e50c7df8e15d6950eab6a4d95b5b07b9d80a3d361f16`.

Checkpoint identity establishes unchanged results over the tested range; it is not an independent proof of all inherited sieve rules. Class-boundary resume tests do not claim recovery from partially written records.

## Reproduce

From this directory with C++17 and Python 3:

```sh
clang++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v10_candidate.cpp -o chomp_sieve_v10_candidate
python3 validate_v10_18.py --solver ./chomp_sieve_v10_candidate --source chomp_sieve_v10_candidate.cpp --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
clang++ -O2 -pthread -std=c++17 test_corner_v10.cpp -o test_corner
./test_corner seed.bin
clang++ -O2 -pthread -std=c++17 test_checkpoint_v10.cpp -o test_checkpoint
mkdir checkpoint-test
./test_checkpoint checkpoint-test
python3 test_preload_resume_v10.py --baseline /path/to/v9_candidate --candidate ./chomp_sieve_v10_candidate --out preload-resume-repeat
```

The generated checkpoints remain preserved locally and are reproducible from the included inputs and runner. They are not duplicated in the compact package. The timing and preload/resume output directories must be new. The timing runner checks input hashes and the complete expected checkpoint hash. The original logs and timing metadata are bundled under `reference18/`, `validation18/`, `reference18-repeat/` and `validation18-repeat/`.
