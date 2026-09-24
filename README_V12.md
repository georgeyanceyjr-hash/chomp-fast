# V12: rectangular and square Chomp

V12 adds rectangular class bounds to V11 while preserving its move generation, lookups and parallel processing. Square runs follow the same traversal when rectangular limits are omitted. Similar square performance is expected; no direct V11-versus-V12 square timing comparison has been performed.

## Verified fresh 10×42 result

- **107,342,199 nonempty P-positions**, published in full.
- **735.729374 seconds (12m 16s)** from fresh analytic input generation through solver exit, 9 workers. No previously computed positions reused.
- Padhi's published solve: **2,141 seconds (35m 41s)**. The ratio of reported times is 2.910; differing hardware, worker configurations and timing scopes prevent a controlled algorithm speed claim.
- Exactly three winning bites, with 1-based (row,column) coordinates: **(5,36), (7,30), (8,26)**.
- Exact agreement with both of Padhi's logged totals after subtracting his empty-board entry; all 420 rectangular opening multiplicities agree. Our entire fresh catalog is byte-identical to our earlier continuation catalog, not to an independently supplied Padhi catalog.

[Padhi's reference log](https://github.com/jaideepsaipadhi/chomp-three-winning-moves/blob/d29cd6f6e2a732e986bd40e986f723824352d85b/triple_verify.log).
This independently reproduces his result; it does not claim discovery of the triple. The interrupted earlier fresh attempt is excluded from timing. The completed run is `fresh-10x42-restart1` in local provenance.

## Downloads

- [Complete catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v12-10x42/chomp_ppositions_10x42_v12.bin.gz)
- [Full restart bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v12-10x42/chomp_v12_10x42_restart.tar.gz)
- [Source and validation](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v12-10x42/chomp_v12_source_and_validation.zip)
- [Manifest](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v12-10x42/10x42_V12_MANIFEST.json)
- [SHA256 checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v12-10x42/SHA256SUMS_V12)

The complete validated [V11 20×20 catalog and restart files](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v11-20x20) remain available and use the same position/checkpoint encodings. All earlier releases are preserved.

## Build and reproduce

Extract the source ZIP or restart TAR into an empty directory. Builds were tested with Apple clang 17, C++17, on Apple Silicon:

```sh
clang++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v12.cpp -o chomp_sieve_v12
clang++ -O3 -march=native -std=c++17 validate_fresh_rectangle.cpp -o validate_fresh_rectangle
python3 reproduce_v12_10x42.py --solver ./chomp_sieve_v12 --validator ./validate_fresh_rectangle --out fresh-run --workers 9
```

The output directory must be new. This generates only the poisoned-square seed and 89 analytic base entries (two-row losing shapes and their transposes, and equal-arm L shapes), then computes all 284 nontrivial classes in height-major order. The runner times input generation through solver exit, validates every checksum, class order and position bound, merges the full sorted catalog and verifies its known SHA256. Compilation and post-run validation/merging are excluded from the reported run time.

Portable small independent checks:

```sh
python3 validate_rectangles_v12.py --solver ./chomp_sieve_v12 --out small-validation
clang++ -O3 -march=native -pthread -std=c++17 test_rectangular_moves.cpp -o test_rectangular_moves
./test_rectangular_moves seed.bin
```

These compare all P-positions in 6×12, 4×35 and 3×42 against an independent row model, and exercise over two million interior moves and 300,000 corner/floor decisions on long encodings and transposes. Original evidence is included in `validation-evidence/`.

## Square runs

The existing square CLI remains compatible:

```sh
python3 fresh_inputs.py square-inputs 20 20
./chomp_sieve_v12 square-inputs/seed.bin 20 NEW-square-checkpoint.bin square-inputs/base.bin 0 0 9
```

Use a new checkpoint for a fresh run. Existing compatible V11 checkpoints can resume the same traversal. When extending an already completed square, preload its complete data and use a new checkpoint with appropriate class limits; a terminal old checkpoint used as the new resume cursor would skip required classes. Extend the analytic base cases to the target bound.

Full arguments: `seed max-width checkpoint preload only-width only-height workers max-height min-width`. The last two are optional; `max-height` defaults to `max-width`, `min-width` to zero. V12 rejects generated class lengths over 63 bits. Do not treat this encoding bound as a claim that all such sizes have been computed or resource-tested.

## Restart and data format

The full restart bundle includes the original minimal seed, analytic base, complete fresh checkpoint, source, tests, logs, timing and manifest. Reconstruct the complete catalog from those files:

```sh
./validate_fresh_rectangle merge base.bin seed.bin checkpoint_10x42_v12.bin 42 10 0
```

Output: `checkpoint_10x42_v12.bin.catalog.bin`. Full catalog SHA256:
`13893537aba7adcf0d5f168d7e9fd9147732c73777021568b18d100051440cb5`.

The checkpoint stores both orientations for lookups; the complete 10×42 catalog includes only shapes with at most 10 rows and 42 columns. CHOMPP02: 8-byte magic, little-endian uint64 count, uint64 maximum key, uint32 width bound, uint32 height bound, then sorted uint64 keys. Catalog size is 858,737,624 bytes before compression. Positions include the poisoned single square and exclude the empty board.

For an interrupted run, verify the checkpoint before resuming. The inherited solver loader does not repair a truncated final record; resume only from a verified complete record boundary after preserving the original file. This release contains a complete checkpoint, not a partial one.
