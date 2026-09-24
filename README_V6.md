# Chomp V6 — validated through 19×19

V6 computed the complete 19×19 catalog from scratch in **882.522304166 seconds (14m 43s rounded)** with 9 workers. V5 took 2273.805030 seconds (37m 54s): **2.5765× faster, 61.19% less wall time** in these single runs on the same Mac with the same seed, base cases and target. Timings include solver startup and checkpoint writing, excluding compilation and post-run hash verification. The solver's own rounded elapsed line is 14m 42s; the reported wall time includes launch/startup overhead.

The full 136-record checkpoint is byte-for-byte identical to V5, SHA-256 `723271af0730f423cd529f462907feb177f210fcc8ac9c38b01d5070fa99ed06`. With the unchanged seed/base cases this yields the same **146,561,777 unique P-positions**. V6 also passed a fresh 18×18 run in 173.807009 seconds, matching all 120 class records exactly.

## Full catalog and restart data

The large data files are unchanged, so V6 deliberately reuses these verified V5 release assets:

- [FULL 19×19 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Complete restart data bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [Data SHA-256 checksums](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/SHA256SUMS)

Catalog gzip SHA-256: `fd8913f6b7224f80d6f515b235f424054186552f9086a6d59d559a2c2c3b92a3`.
Restart bundle SHA-256: `4c8f04f146a0577fdf48dfe19199a7352111e28f5a74a67f80a3354874eadf51`.

The V6 source package is an upgrade, not a duplicate data archive. Download the restart data bundle above and use V6 source instead of the bundled V5 source. [README_V5.md](README_V5.md) documents the unchanged formats and validation scope.

## Build and run

The published `chomp_sieve_v6.cpp` is byte-identical to the tested candidate, including its original experimental comment and usage label. This release and manifest record its successful full validation.

```sh
c++ -O3 -march=native -DNDEBUG -pthread -std=c++17 chomp_sieve_v6.cpp -o chomp_sieve_v6
# Fresh 19×19 validation; fresh19.bin must not already exist.
./chomp_sieve_v6 seed.bin 19 fresh19.bin base.bin 0 0 9 > fresh19.log
```

## Continue to 20×20 later

20×20 has NOT been started. Extract the complete restart bundle into a directory, add/build V6 there, then when desired:

```sh
python3 prepare_resume_v5.py seed.bin checkpoint.bin preload20.bin --dimension 20
./chomp_sieve_v6 seed.bin 20 checkpoint20.bin preload20.bin 20 0 9 > sieve20.log
```

Both output paths must initially be absent. `preload20.bin` includes the complete old catalog and base cases through 20; `checkpoint20.bin` is a NEW checkpoint for width 20 only. Never use the old terminal 19×18 checkpoint as the output/resume cursor: that would skip earlier 20-wide classes. Preserve seed, preload and new checkpoint together. Do not feed the CHOMPP02 merged catalog to the CHOMPP01 seed reader. The preparation workflow was tested on 7×7→8×8; V6's full checkpoint format and content match V5 exactly.

## Optimization

Precomputed corner-move components avoid repeated population counts and masks. During each immutable worker phase, workers also reject candidates having an existing corner hit. Misses are retried in the original sequential order, preserving dependencies on newly discovered positions. No shared database writes occur during the parallel phase. All exact lookup and class-order rules remain unchanged.

Full checkpoint identity validates agreement with V5, not an independent mathematical proof of the inherited sieve or square-skeleton theorem. No speed prediction for 20×20 is claimed.
