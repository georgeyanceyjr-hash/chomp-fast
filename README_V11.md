# Chomp V11 — complete 20×20 catalog

**451,253,296 unique P-positions**, including all **146,561,777** positions from 19×19 and **304,691,519** additional positions.

The **19-to-20 continuation took 1,344.416611708 seconds (22 minutes 24 seconds rounded)** with 9 workers. This includes loading the complete 19×19 input, solving the 17 new width-20 classes, checkpoint writes and exit. It excludes preparation and final catalog validation/merging. It is not a fresh 20×20 timing.

## Downloads

- [FULL 20×20 catalog, gzip](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v11-20x20/chomp_ppositions_20x20_v11.bin.gz)
- [Complete restart bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v11-20x20/chomp_v11_20x20_restart.tar.gz)
- [Small source and validation package](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v11-20x20/chomp_v11_source_and_validation.zip)
- [Release checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v11-20x20/SHA256SUMS_V11)
- [Manifest](20x20_V11_MANIFEST.json) and [measured timing](timing-report-v11-20x20.md)

The full catalog is **3,610,026,400 bytes** after decompression. It contains every recorded P-position with width and height at most 20, including the seed and base cases. Its SHA256 is:

`b2d8e2bda7c158162fc61f14f294d5deeba0acbe96c1fd62ba1e765924a88ec4`

```sh
# Check the downloaded files against their entries in SHA256SUMS_V11.
shasum -a 256 chomp_ppositions_20x20_v11.bin.gz chomp_v11_20x20_restart.tar.gz
# Decompress without deleting the original download.
gzip -dc chomp_ppositions_20x20_v11.bin.gz > chomp_ppositions_20x20_v11.bin
shasum -a 256 chomp_ppositions_20x20_v11.bin
mkdir v11-restart
tar -xzf chomp_v11_20x20_restart.tar.gz -C v11-restart
cd v11-restart
shasum -a 256 -c SHA256SUMS_RESTART
```

## V11 change and exact source provenance

V11 widens both Bloom-filter capacity counters, `bloom_bits` and `wanted`, from 32 to 64 bits. The previous power-of-two growth loop could wrap beyond 178,956,970 entries in one class. Move generation, membership decisions and checkpoint encoding are unchanged. This is a capacity correction, not another optimization search.

The run began under the working name **V10 capacity64** and was named **V11** while computing. `chomp_sieve_v11.cpp` is byte-for-byte the actual tested source. Its historical V10 comment and usage message are deliberately preserved. The manifest maps all historical names to release names. Source SHA256:

`5ce78972e5d9f6382fa870a516624e12560db00b030b9d7df2528973d8993244`

The 64-bit correction passed a complete fresh 18×18 run with exact reference checkpoint SHA256 `a0cb64b2783f8a650e16ab25f8ab0696dbe3e6789728ac2d9fdcc213954e5b8a`. A 7×7-to-8×8 continuation reproduced all 322 independently solved 8×8 P-positions before the large run.

## Data and formats

| Component | Positions | Purpose |
|---|---:|---|
| `seed.bin` | 102 | Original independently computed CHOMPP01 bootstrap |
| `base20.bin` | 34 | Narrow-board and square-skeleton cases through 20 |
| Historical 19×19 records inside `preload20.bin` | 146,561,644 | All 136 completed 19×19 class records |
| `checkpoint20.bin` | 304,691,516 | All 17 new classes R(20,3) through R(20,19), including transposes |
| Complete merged catalog | **451,253,296** | Disjoint union of the four components above |

`preload20.bin` contains the 34 base cases plus the 136 historical records, so it has 137 records. Do not add `base20.bin` a second time when merging. Three of the base cases are new beyond 19×19.

The new width-20 checkpoint is **2,437,532,544 bytes**, SHA256:

`7b94bb92f3474bc70014b176a117420d473a2985e77d1586c72ef9f1af6c16c3`

The complete catalog uses little-endian `CHOMPP02`: 8 magic bytes, uint64 count, uint64 maximum key, uint32 width limit, uint32 height limit, followed by sorted unique uint64 boundary keys. Boundary steps use 1=right and 0=down, normalized to leading 1 and trailing 0. The poisoned singleton is included as a P-position. This catalog is not a CHOMPP01 seed and must not be passed directly to the solver's seed reader.

## Build and reproduce the measured continuation

The restart bundle is self-contained: original seed, base20, full preload20, full checkpoint20, exact source, validator, checks, logs, timing and manifest. Requires a C++17 compiler and Python 3; the measured build used Apple clang 17 with the flags below, on a 24 GB Mac.

```sh
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v11.cpp -o chomp_sieve_v11
python3 reproduce_v11_20.py --solver ./chomp_sieve_v11 --output repeat20 --workers 9
```

`repeat20` must not exist. The runner verifies the supplied seed and preload before launch, uses a NEW checkpoint, computes only width 20 and checks the entire resulting checkpoint SHA256. The equivalent direct solver command is:

```sh
./chomp_sieve_v11 seed.bin 20 NEW-checkpoint20.bin preload20.bin 20 0 9 > NEW-sieve20.log
```

Never use the old terminal R(19,18) checkpoint as this output/resume file: that would skip earlier 20-wide classes. For recovery from an interrupted width-20 run, preserve its seed, preload and output checkpoint together. Before resuming, check all complete record checksums and remove any incomplete trailing record in a separate recovery copy; the inherited loader stops at incomplete data but does not truncate it. Reuse only that checked output checkpoint with the same width-20 command.

To rebuild and fully validate the published catalog without recomputing Chomp:

```sh
c++ -O3 -std=c++17 validate_merge_v11.cpp -o validate_merge_v11
./validate_merge_v11 seed.bin preload20.bin checkpoint20.bin rebuilt20.bin
shasum -a 256 rebuilt20.bin
```

The rebuilt hash must equal the complete catalog hash above. Allow several gigabytes of working memory and disk space for the merge. Do not overwrite the original catalog.

## Reproduce the smaller independent checks

```sh
python3 bootstrap_validation_v11.py --output-dir bootstrap-new
python3 test_continuation_v11.py --solver ./chomp_sieve_v11 --out continuation-new
c++ -O2 -pthread -std=c++17 test_corner_v11.cpp -o test_corner
./test_corner seed.bin
# Optional full reference-range check, with a new output directory:
python3 validate_v11_18.py --solver ./chomp_sieve_v11 --source chomp_sieve_v11.cpp --seed seed.bin --base base18.bin --output validation18-new --workers 9
```

## Validation scope and future use

Every checkpoint checksum, strict per-record order, dimension bound, complete new class sequence, terminal R(20,19), and absence of overlap in the full catalog passed. The complete binary catalog and restart files are supplied, not samples. Checksums establish integrity; the inherited mathematical sieve exclusions have not received a new proof here. No independent full 20×20 catalog comparison has been performed.

Preserve all files for a possible future extension. A larger dimension would require extended base cases, the entire accumulated database and a NEW output cursor that includes its earlier wide classes. This release does not start or benchmark 21×21. Previous releases remain available, and uncorrected V4 results are not treated as valid.
