# Current release: Chomp V5

The complete **19×19 catalog contains 146,561,777 unique P-positions**, computed from scratch in **37m 54s** with 9 workers.

- [Download the full catalog and restart package](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v5-19x19)
- [V5 build, validation and future 20×20 restart instructions](README_V5.md)
- [Exact results manifest](19x19_V5_MANIFEST.json) and [SHA-256 checksums](SHA256SUMS)

V5 fixes a 32-bit population count in boundary transforms. Use V5 for new work. The historical V2–V4 claims below are retained as history and are **superseded, not revalidated by V5**. Some historical files mentioned below are not distributed in this repository.

---

# Chomp Fast

This is a modern, compact solver built from George Yancey's original Sage
Chomp solver and P-position catalog.

It solves a supplied position by generating each legal child once and checking
that child against a compact Bloom-filtered binary P-position database. The
Bloom filter rejects almost all non-P children in constant time; the sorted
integer catalog then confirms the small number of possible matches. The file contains
32-bit integers rather than Python strings, shrinking the 53 MB uncompressed
catalog to about 6 MB.

## Build

```bash
python3 build_ppositions.py chomp_ppositions.sobj ppositions.bin
g++ -O3 -march=native -std=c++17 chomp_fast.cpp -o chomp_fast
```

## Use

```bash
./chomp_fast 1111000
```

For a persistent process that avoids startup cost and solves many positions,
send one binary position per line to batch mode:

```bash
./chomp_fast --batch
```

Binary notation is the same as the original solver: trace the board's upper
right boundary, using `1` for a step right and `0` for a step down. Leading
zeros and trailing ones are cleaned automatically. Coordinates are one-based,
with `(1,1)` the poisoned lower-left square.

The supplied historical catalog has 1,545,962 P-positions, is complete through
28-bit boundary strings, and then continues partway into length 29. The program
conservatively rejects inputs beyond the last recorded P-position.

## Winning Domain Sieve extension

`chomp_sieve.cpp` is the 64-bit extension generator based on George Yancey's
Winning Domain Sieve. It processes only the long complex half of the congruence
class array, inserts conjugates automatically, applies the basic sieve, then
tests height expansion, width expansion, and same-class corner domains in that
order. Its output has been validated by withholding a solved range and
regenerating it with zero missing or extra P-positions.

The completed `17x17` catalog contains 11,213,698 P-positions. The non-square
classes through `17x16` are explicitly sieved; square classes use the proven
square-skeleton result.

### Optimized parallel sieve

`chomp_sieve_parallel.cpp` preserves the same class order and exact lookup
semantics while improving the expensive catalog-query path. It uses a compact
open-addressed 64-bit set, a 16 MiB three-probe blocked Bloom filter, and
constant-time boundary cleaning. Width and height expansion tests are evaluated
in parallel in bounded chunks; same-class corner tests remain sequential and in
the original candidate order, preserving the sieve dependency.

```bash
g++ -O3 -march=native -flto -DNDEBUG -pthread -std=c++17 \
  chomp_sieve_parallel.cpp -o chomp_sieve_parallel
```

The optional final argument selects the worker count. On the 9-core benchmark
host, a full-catalog replay of class `17x13` fell from 76.6 seconds for the
original implementation to 29.1 seconds. The sorted checkpoints were byte-for-
byte identical. This is a 2.63x end-to-end speedup for that benchmark; other
classes and machines will vary.

### V2 and definitive 18x18 result

`chomp_sieve_v2.cpp` is the current production implementation. It explicitly
rejects the zero key, shards exact additions by congruence class, uses one-hash
blocked Bloom probing, balances expansion work in cyclic blocks, and writes
append-only binary `CHOMPCP2` class deltas with checksums. On the full-catalog
`17x13` replay it required 23.8 seconds, an additional improvement of about 18%
over the first parallel version.

A clean 9-core V2 run from the historical seed reached `18x18` in 55 minutes
42 seconds and produced 38,331,741 total catalog entries. The terminal new
long-half counts are 4,822,723 for `R(18,16)` and 4,756,890 for `R(18,17)`.
See `18x18_V2_CLEAN_MANIFEST.txt` for hashes and full validation details.

Important: the earlier files named `chomp_ppositions_18x18.bin` and
`18x18_PRODUCTION_MANIFEST.txt` are invalid and superseded. A zero-sentinel bug
was discovered during V2 development. Use only
`chomp_ppositions_18x18_v2_clean.bin` for 18x18 results.

### V3 class-local filtering

`chomp_sieve_v3.cpp` is the recommended production implementation. It replaces
V2's increasingly saturated global Bloom filter with independently resized
per-class Bloom filters and pre-reserves likely class-table capacity. The
mathematical order and exact confirmation rules are unchanged.

A fresh 9-core V3 run reached `18x18` in 34 minutes 16 seconds, compared with
55 minutes 42 seconds for V2, a 38.5% end-to-end reduction. Its checkpoint and
catalog are byte-for-byte identical to V2. See
`18x18_V3_CLEAN_MANIFEST.txt` for complete timings and hashes. The V2 catalog
remains authoritative and identical; V3 is preferred for future computation.

### V4 fixed-class corner path and PGO

`chomp_sieve_v4.cpp` specializes the dominant interior-corner path. Such a move
preserves the boundary string's leading one, trailing zero, width, and height,
so V4 skips redundant normalization and probes the already-known class shard
directly. The one class overlapping the immutable seed frontier retains the
general seed-aware lookup. Boundary expansion moves also route directly to
their known child class. The production binary was additionally built with GCC
profile-guided optimization trained on `R(17,13)`.

The corrected 9-thread production reached 17x17 in 8 minutes 48 seconds. A
terminal interruption then stopped the unfinished `R(18,16)` class; resuming
from the last checksummed class completed the remaining two classes in 19
minutes 32 seconds. Accepted completed segments therefore total 28 minutes 20
seconds, including checkpoint reload, versus 34 minutes 16 seconds for V3—a
17.3% reduction. The V4 checkpoint and merged 38,331,741-entry catalog are
byte-for-byte identical to V2 and V3. See `18x18_V4_CLEAN_MANIFEST.txt` for the
validation history, the rejected frontier-routing experiment, hashes, and
build details.
