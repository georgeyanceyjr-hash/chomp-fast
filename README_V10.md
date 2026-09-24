# Chomp V10 — validated through 18×18

V10 is the final planned speed-improvement pass. Two fresh 18×18 runs averaged **64.743236 seconds**, compared with **66.133774 seconds** for V9: **2.10% less time**. Runs used the same machine, fresh inputs, compiler flags and 9 workers, in order V9, V10, V10, V9. This is a modest gain based on a limited sample. Timing includes input loading, solving, checkpoint writing and exit; compilation and final hashing are excluded.

| Run | V9 | V10 |
|---|---:|---:|
| First |65.808957 s|64.636945 s|
| Second |66.458591 s|64.849527 s|
| Mean |66.133774 s|64.743236 s|

All four complete 303,952,504-byte checkpoints match the reference exactly: 120 records ending at R(18,17), with 37,993,702 checkpoint positions, 102 original seed positions and 28 base positions: **37,993,832 unique P-positions**.

**V10 is validated through 18×18; its full 19×19 time is unmeasured.** V9 remains the latest full 19×19 validated solver, completing in **387.906746 seconds (6m28s)** with the exact V5/V8 checkpoint and **146,561,777 total unique positions**. See [the V9 full 19 timing report](timing-report-v9-19x19.md).

## Build and reproduce

Download [the source and validation bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v10-18x18/chomp_v10_source_and_validation.zip). It contains both the exact timed V10 source and final source, V9 baseline source, seed/base18, four timing records/logs, checks, independent 8×8 fixture, manifest and hashes. The generated 304MB checkpoints are recreated and completely hash-checked by the runner.

The final source differs from the timed source only in the executable name printed for invalid arguments. Solver logic is unchanged; `label-only-change.json` records the exact replacement. The final binary also passed all 11 preload/resume cases. Separate hashes identify both sources.

```sh
shasum -a 256 -c SHA256SUMS_PACKAGE
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v10_candidate.cpp -o chomp_sieve_v10_candidate
python3 validate_v10_18.py --solver ./chomp_sieve_v10_candidate --source chomp_sieve_v10_candidate.cpp --seed seed.bin --base base18.bin --output validation18-new --workers 9
```

The output directory must be new. To reproduce the exact timed build, substitute `chomp_sieve_v10_timed.cpp` and its corresponding executable/source argument. The measured compiler was Apple clang 17 with the flags shown above.

- Final source SHA256: `2e0ce1430a1e5d59cd424c50504687aea693ed11f157fd1f5dbaad7cb2b725b7`
- Exact timed source SHA256: `6a0d59c02e7d680988d4e50c7df8e15d6950eab6a4d95b5b07b9d80a3d361f16`
- Full 18 checkpoint SHA256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`

## Changes and checks

V10 stops increasing corner queries after their keys exceed the class maximum, defers transpose insertion until class completion, and sorts only transpose deltas before merging checkpoint output. Candidate/class order, exact membership and the CHOMPCP2 format are preserved. [Implementation reasoning](implementation-notes.md) and [measured report](speed-report-v10.md) provide details.

Checks cover 3,344,280 independent row-model moves, 300,000 corner/floor decisions, 65 serializer records with 432,160 values, 11 preload/restart cases and the independent 322-position 8×8 fixture. Sanitizers checked 257-item batches with 1, 9 and 17 workers. All four full 18 checkpoints match exactly. Agreement over the tested range does not independently prove every inherited sieve rule; restart checks cover completed class records rather than torn writes.

```sh
c++ -O2 -pthread -std=c++17 test_corner_v10.cpp -o test_corner
./test_corner seed.bin
c++ -O2 -pthread -std=c++17 test_checkpoint_v10.cpp -o test_checkpoint
mkdir checkpoint-test
./test_checkpoint checkpoint-test
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v9_candidate.cpp -o chomp_sieve_v9_candidate
python3 test_preload_resume_v10.py --baseline ./chomp_sieve_v9_candidate --candidate ./chomp_sieve_v10_candidate --out preload-resume-new
python3 bootstrap_validation_v10.py --output-dir bootstrap-new
```

The bootstrap script independently reproduces the included 8×8 fixture. For sanitizer reproduction compile `test_scheduling_v10.cpp` with `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -pthread -std=c++17`. Run its seed/base inputs at dimension 8 with a new checkpoint and 1, 9 or 17 workers. Expected checkpoint SHA256 is `9e1779f0056efce30ac104fb10fb321afb649a467d314c3e5c257859c6ccd491`.

## Complete 19×19 data and future continuation

The full 146,561,777-position catalog and complete restart files remain the verified V5 release assets, with V9 full-checkpoint agreement. They are not a full 19 V10 result:

- [FULL 19×19 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Original seed/base, complete checkpoint, logs and restart files](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [Data checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/SHA256SUMS)
- [Tested future 20×20 continuation instructions](README_V6.md)

A future 20×20 run must extend base cases through 20 and preload 19×19 data into a NEW output checkpoint. Using terminal R(19,18) as the resume cursor would skip earlier 20-wide classes. No 20×20 computation has been started. Earlier releases remain available; uncorrected V4 results are not validated.
