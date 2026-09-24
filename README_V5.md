# Chomp V5 — complete 19×19 catalog

Completed September 24, 2026 from a newly generated seed, using 9 workers.
**146,561,777 unique P-positions; wall-clock time 37m 53.805030s** (37m 54s rounded).
The final class R(19,18) contributed 29,940,283 positions before conjugates.
The 18×18 coverage milestone was reached at 9m 32s during this 19-wide traversal; this includes earlier 19-wide work and is not a standalone 18×18 benchmark.

V5 fixes a 32-bit population count in the boundary move transform to use 64 bits. Historical V4 results are superseded; do not use them as verified V5 input.

## Downloads and integrity

The release includes the FULL catalog as `chomp_ppositions_19x19_v5.bin.gz`, plus a restart bundle containing the full checkpoint, seed, base cases, portable C++ source, validation utilities, log, manifest and this guide. Verify `SHA256SUMS` before use. The catalog contains every recorded P-position in bounds, not a sample or just terminal classes.

The decompressed catalog is 1,172,494,248 bytes. Its little-endian format is:
8 bytes `CHOMPP02`; uint64 count; uint64 maximum key; uint32 width limit; uint32 height limit; then `count` sorted unique uint64 keys. Keys encode the board boundary with 1=right and 0=down, normalized to leading 1/trailing 0. The poisoned singleton is a P-position. Square skeleton cases and narrow-board cases are included.

`seed.bin` is CHOMPP01 with 102 independently computed positions complete through boundary length 12. `base.bin` holds 31 additional narrow-board and square-skeleton positions. `checkpoint.bin` has 136 checksummed CHOMPCP2 records and 146,561,644 positions, including conjugates. The disjoint union has 146,561,777 entries.

## Build and reproduce 19×19

Run in an extracted restart-bundle directory. Requires Python 3 and a C++17 compiler.

```sh
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v5.cpp -o chomp_sieve_v5
./chomp_sieve_v5 seed.bin 19 fresh19.bin base.bin 0 0 9 > fresh19.log
```

Use a NEW checkpoint filename for a fresh run. To resume an interrupted run, reuse its checkpoint only after checking it has no incomplete trailing record. The original reader stops at incomplete records but does not truncate them; recover the file to its last fully checksummed boundary before resuming.

To reproduce the full catalog and validate all stored record checksums, ordering and bounds:

```sh
c++ -O3 -std=c++17 validate_merge_v5.cpp -o validate_merge_v5
./validate_merge_v5 seed.bin base.bin checkpoint.bin rebuilt19.bin
```

## Continue toward 20×20 later

The 20×20 computation has NOT been started. Prepare and then run only when desired:

```sh
python3 prepare_resume_v5.py seed.bin checkpoint.bin preload20.bin --dimension 20
./chomp_sieve_v5 seed.bin 20 checkpoint20.bin preload20.bin 20 0 9 > sieve20.log
```

`checkpoint20.bin` must initially not exist. The preparation utility copies the complete 19×19 checkpoint and generates base cases through 20. The full data is a PRELOAD, not the resume cursor. The width filter `20` computes R(20,3) through R(20,19) in order. Using the old terminal R(19,18) checkpoint as the output/resume file would incorrectly skip earlier 20-wide classes. Preserve seed.bin, preload20.bin, and checkpoint20.bin together; the 20-only checkpoint is not a standalone full catalog. Do not feed the CHOMPP02 merged catalog to this solver directly: its seed reader expects CHOMPP01.

The continuation procedure was tested on 7×7→8×8 and exactly matched all 322 independently solved 8×8 P-positions. No timing or memory estimate for 20×20 is promised.

## Validation

All 136 record checksums, strict per-record ordering, class membership, dimension bounds, class order, terminal class and absence of overlaps passed. Independent exhaustive play matched all 322 8×8 P-positions. An independent row-based move model matched 1,993,338 randomly generated 19×19-board moves.

```sh
c++ -O2 -pthread -std=c++17 test_moves_v5.cpp -o test_moves
./test_moves
python3 bootstrap_validation.py --output-dir bootstrap-test
```

The square-skeleton theorem and the V4 sieve's mathematical exclusions remain inherited assumptions; checksums establish file integrity, not a new proof of the sieve. The reported catalog is the completed V5 computation, validated as described above.
