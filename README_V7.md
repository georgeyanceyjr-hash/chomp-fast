# Chomp V7 — prerelease validated through 18×18

V7 completed a fresh **18×18 run in 111.843081583 seconds (1m 52s rounded)** using 9 workers. The earlier equivalent V6 run took 173.807009 seconds (2m 54s): **35.65% less time, or 1.554× as fast**. These are single-machine measurements; V6 was not rerun as a paired trial. Timing includes solver launch, input loading, solving, checkpoint writing and exit, and excludes compilation and post-run hashing.

All **120 checkpoint records** match validated V5/V6 byte-for-byte. The checkpoint stores 37,993,702 positions; the unchanged 102 seed positions and 28 dimension-18 base positions give **37,993,832 unique P-positions**. The terminal class is R(18,17).

**Validation stops at 18×18.** V7 has no measured or validated full 19×19 run. [V6 remains the full 19×19 validated release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v6-19x19). No 20×20 computation was performed.

## Included source and reproducible inputs

Download and extract `chomp_v7_source_and_validation.zip`. It contains the exact tested source, original `seed.bin`, the original `base18.bin`, validation runner, boundary test, independent bootstrap solver, logs, status, timing report and checksums. These inputs are sufficient to reproduce a fresh 18×18 run without any external data download. The generated 304 MB checkpoint is not duplicated in this source package; the runner recreates it and checks its complete SHA-256.

The source filename and original experimental header are retained so its bytes stay identical to the tested candidate:

- Source SHA-256: `352af0da067d07ecc8816362f56f75b92d6fb8ff1d3264dcee1e007c1cb36466`
- Expected 18×18 checkpoint SHA-256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`

## Build and reproduce 18×18

From the extracted package directory, use a C++17 compiler and Python 3. The output directory must not exist, which prevents accidental resume or overwrite.

```sh
shasum -a 256 -c SHA256SUMS_PACKAGE
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v7_candidate.cpp -o chomp_sieve_v7_candidate
python3 validate_v7_18.py --solver ./chomp_sieve_v7_candidate --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
```

The recorded build used Apple clang 17.0.0 with the flags above. The runner checks both input hashes before launch and verifies the complete resulting checkpoint against V5/V6 after recording the runtime. The measured original run is preserved in `validation18/status.json` and `validation18/sieve.log`; machine-specific command paths in status are replaced by relative equivalents.

The boundary test reproduces 8,912,898 exhaustive old-transform comparisons and 1,497,756 independent row-model comparisons through dimensions 31:

```sh
c++ -O3 -pthread -std=c++17 test_boundary_v7.cpp -o test_boundary_v7
./test_boundary_v7
```

The independent 8×8 check regenerates reference positions with a row-based recursive solver and compares all 322 positions plus the recorded checkpoint hash:

```sh
python3 bootstrap_validation.py --output-dir bootstrap-repeat
python3 verify_v7_8.py --solver ./chomp_sieve_v7_candidate --expected bootstrap-repeat/expected8.json --output validation8-repeat
```

## Changes and validation scope

V7 uses smaller batches (16,384 candidates), direct boundary-move formulas, and a lower bound that omits repeat corner lookups already disproved by the parallel phase. It also removes the redundant membership lookup before insertion and reuses the same hash for Bloom and exact lookup. Original joined worker threads and their write barrier are retained. See `speed-report.md` and `correctness-review.txt` for details.

The final source passed all 322 independent 8×8 P-positions, boundary-transform comparisons, and the full 18×18 checkpoint comparison. The separate pool stress variant was tested with ASan/UBSan, 32-position batches and 1/9/64 workers; that pool variant is not the published implementation. Checkpoint identity confirms agreement with the validated reference, not an independent mathematical proof of the inherited sieve.

## Existing full 19×19 catalog and future restart

The complete validated 19×19 data remains available in the unchanged V5 release assets and is compatible with V6:

- [FULL 19×19 P-position catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Complete 19×19 restart data](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [Data SHA-256 checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/SHA256SUMS)
- [V6 source, full-run timing and future 20×20 instructions](https://github.com/georgeyanceyjr-hash/chomp-fast/blob/main/README_V6.md)

Those assets contain **146,561,777 unique 19×19 P-positions**. They are V5/V6 results, not evidence of a V7 full 19×19 run. For a future 20×20 continuation, use the V6 instructions: preload the old data into a new run, extend base cases through dimension 20, and never use the terminal 19×18 checkpoint as the new resume cursor. No earlier V4 result is asserted valid by this prerelease.
