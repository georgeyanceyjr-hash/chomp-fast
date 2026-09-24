# Chomp V9 — prerelease validated through 18×18

Fresh **18×18 in 66.420068125 seconds (1m 6s)** with 9 workers. V8 run immediately beforehand took **73.355474125 seconds**: V9 used **9.45% less time**, or **1.104× as fast**. Both used the original seed, dimension-18 base, same machine and compiler flags. These are single measured runs. Wall time includes loading, solving, checkpoint writing and exit; compilation and final hash checking are excluded.

The full checkpoint is byte-identical to V5/V6/V7/V8: **120 records**, ending at **R(18,17)**, with **37,993,702 checkpoint positions**. Adding 102 seed and 28 base positions gives **37,993,832 unique P-positions**.

**This V9 prerelease is validated through 18×18 only. Its full 19×19 timing has not yet been measured.** V8 subsequently completed full 19×19 in **416.4578645 seconds (6m 56s)** with the exact V5 checkpoint; see [that measured report](timing-report-v8-19x19.md). The older V8 release retains its original 18×18 release scope. No 20×20 computation has been started.

## Build and reproduce

Download and extract [chomp_v9_source_and_validation.zip](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v9-18x18/chomp_v9_source_and_validation.zip). It contains the exact tested source, original seed/base18, validation scripts, independent tests, logs, timing records, manifest and checksums. The generated 304 MB checkpoint is not bundled; the runner recreates the complete file and verifies its entire SHA-256.

- Source SHA-256: `6b42adb8956f1ee107ea80b4ddcf38428fc20de389894deb7c1635f67cb7b8ab`
- Full 18×18 checkpoint SHA-256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`

From the extracted directory:

```sh
shasum -a 256 -c SHA256SUMS_PACKAGE
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v9_candidate.cpp -o chomp_sieve_v9_candidate
python3 validate_v9_18.py --solver ./chomp_sieve_v9_candidate --source chomp_sieve_v9_candidate.cpp --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
```

The output directory must be new. Input hashes are checked before launch; the runner refuses to resume or overwrite an existing run. The measured build used Apple clang 17.0.0 and the flags above. The candidate filename and source bytes are preserved exactly.

## Improvements and validation

V9 keeps three changes: a cheaper one-multiply hash, exact class minimum/maximum bounds that reject impossible lookups before hashing, and direct generation of candidates surviving the existing basic sieve. Every Bloom-filter hit still receives exact membership checking; candidate order and checkpoint format are unchanged. See [the measured report](speed-report-v9.md) and [manifest](18x18_V9_MANIFEST.json).

Checks cover 848 generation parameter cases and 12,028,876 emitted candidates in the original order, 10,000 inserted keys and 200,000 negative exact queries, 3,344,280 independent row-model interior moves and 300,000 corner/floor decisions. Address/undefined-behavior sanitizer runs with 257-item batches and 1, 9 and 17 workers all matched the independent 322-position 8×8 checkpoint. Reproduce the independent checks:

```sh
python3 check_generation_v9.py
c++ -O2 -pthread -std=c++17 test_generation_v9.cpp -o test_generation_v9
./test_generation_v9
c++ -O2 -pthread -std=c++17 test_membership_v9.cpp -o test_membership_v9
./test_membership_v9
c++ -O2 -pthread -std=c++17 test_corner_v9.cpp -o test_corner_v9
./test_corner_v9 seed.bin
python3 bootstrap_validation_v9.py --output-dir bootstrap-repeat
python3 verify_v9_8.py --solver ./chomp_sieve_v9_candidate --expected bootstrap-repeat/expected8.json --output validation8-repeat
```

`test_scheduling_v9.cpp` differs from the tested source only in the batch-size constant. Compile it with `-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -pthread -std=c++17` and run fresh 8×8 checkpoints with workers 1, 9 and 17. The expected checkpoint hash is `9e1779f0056efce30ac104fb10fb321afb649a467d314c3e5c257859c6ccd491`. Exact agreement with the reference does not independently prove the inherited sieve.

## Existing complete 19×19 data

The **146,561,777-position** catalog and full restart data remain unchanged V5 assets, with full-checkpoint agreement from V6 and V8. They are not a V9 full 19×19 result:

- [FULL 19×19 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Complete restart bundle: original seed/base and full checkpoint](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [Data checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/SHA256SUMS)
- [Tested future 20×20 continuation instructions](https://github.com/georgeyanceyjr-hash/chomp-fast/blob/main/README_V6.md)

A future 20×20 continuation must extend base cases through 20 and preload the 19×19 data into a NEW output checkpoint. Using terminal R(19,18) as the resume cursor would skip earlier 20-wide classes. Previous releases remain available; no V4 results are asserted valid.
