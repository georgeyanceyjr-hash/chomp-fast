# Chomp V8 — prerelease validated through 18×18

Fresh **18×18 in 74.092060834 seconds (1m 14s)** with 9 workers, compared with V7’s earlier 111.843081583 seconds (1m 52s): **33.75% less time, 1.510× as fast**. Both used the same fresh seed and dimension-18 base. These are single-machine measurements; the full V7 run was not repeated as a paired trial. Wall time includes startup, loading, solving, checkpoint writing and process exit, excluding post-run hashing.

All **120 checkpoint records** are byte-for-byte identical to V5/V6/V7. The checkpoint’s 37,993,702 entries plus 102 seed and 28 base positions give **37,993,832 unique P-positions**, ending at R(18,17).

**V8 is validated through 18×18 only.** It has not completed a full 19×19 validation. [V6 remains the complete 19×19 validated implementation](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v6-19x19). No 20×20 computation has been started.

## Source and validation package

Extract `chomp_v8_source_and_validation.zip`. It includes the exact tested source, original seed and base18, reproduction runner, tests, logs, manifest and checksums. It is self-contained for a fresh 18×18 run. The generated 304 MB checkpoint is not duplicated in the compact package; the runner reproduces it and checks its entire SHA-256.

- Source SHA-256: `6360e13e0d7c08811e587ef5c8e0a6695ca0a28e25ff1183d35dd79a164c0d9a`
- Expected checkpoint SHA-256: `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`

The candidate filename and experimental header are retained to preserve the tested bytes. From the extracted directory:

```sh
shasum -a 256 -c SHA256SUMS_PACKAGE
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v8_candidate.cpp -o chomp_sieve_v8_candidate
python3 validate_v8_18.py --solver ./chomp_sieve_v8_candidate --seed seed.bin --base base18.bin --output validation18-repeat --workers 9
```

Use a new output directory; the runner refuses to resume or overwrite it. Input hashes are checked before launch. The recorded build used Apple clang 17.0.0 and the flags above. Original timing and log are in `validation18`; status paths have been made relative and the process ID removed.

## Improvements and checks

V8 directly enumerates corner moves and prunes groups proven below the batch floor, tests four Bloom-filter bits in one word before exact lookup, distributes immutable work in 128-candidate blocks, and replaces a combination-generator division with a shift. Sequential insertion order and checkpoint format are unchanged. Batch size remains 16,384. See [the timing report](speed-report-v8.md).

The final source matched all 322 independently solved 8×8 positions. Corner checks compared 300,000 decisions against 3,344,280 independent row-model moves; lookup tests cover inserts, duplicates, growth and 200,000 negative queries. ASan/UBSan tests with 257-item batches and 1, 9 and 17 workers produced the identical 8×8 checkpoint. The scheduling test source changes only the batch size for stress coverage.

```sh
c++ -O3 -pthread -std=c++17 test_corner_v8.cpp -o test_corner_v8
./test_corner_v8 seed.bin
c++ -O3 -pthread -std=c++17 test_membership_v8.cpp -o test_membership_v8
./test_membership_v8
python3 bootstrap_validation.py --output-dir bootstrap-repeat
python3 verify_v8_8.py --solver ./chomp_sieve_v8_candidate --expected bootstrap-repeat/expected8.json --output validation8-repeat
```

Exact agreement with the reference does not independently prove the inherited sieve.

## Existing complete 19×19 data

The existing **146,561,777-position** catalog and restart data remain unchanged V5/V6 results, not a V8 full 19×19 result:

- [FULL 19×19 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Complete restart bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [Data checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/SHA256SUMS)
- [V6 and future 20×20 continuation instructions](https://github.com/georgeyanceyjr-hash/chomp-fast/blob/main/README_V6.md)

A future 20×20 continuation must extend base cases through 20 and preload old results into a NEW output checkpoint; using terminal R(19,18) as its resume cursor would skip earlier 20-wide classes. No earlier V4 result is asserted valid here.
