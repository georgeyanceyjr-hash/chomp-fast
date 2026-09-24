# Chomp fast

## Latest prerelease: V9 — validated through 18×18

Fresh **18×18 in 66.420 seconds**, versus **73.355 seconds** for V8 run immediately beforehand: **9.45% less time**. Both used 9 workers and identical fresh inputs. All 120 checkpoint records match exactly, covering **37,993,832 unique P-positions**. V9 full 19×19 timing has not yet been measured.

- [V9 prerelease](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v9-18x18)
- [V9 source, build and self-contained validation](README_V9.md)
- [V9 measured timing report](speed-report-v9.md)

## Latest full 19×19 validation: V8

After its 18×18 prerelease, V8 completed a fresh **19×19 in 416.4578645 seconds (6m 56s)** with 9 workers. Its complete checkpoint is byte-identical to validated V5, confirming **146,561,777 unique P-positions** including seed and base. This was **52.81% less time than V6**. The historical V8 release retains its original 18×18 label.

- [V8 full 19×19 measured timing and exact checkpoint hash](timing-report-v8-19x19.md)
- [Exact tested V8 source](chomp_sieve_v8_candidate.cpp)
- [Original V8 prerelease](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v8-18x18)
- [FULL 19×19 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Complete restart data, unchanged V5 assets](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [V6 build and tested future 20×20 continuation instructions](README_V6.md)

## Earlier versions

V6 completed fresh 19×19 in **14m 43s**, versus V5's **37m 54s**, with byte-identical results. V7 completed fresh 18×18 in **1m 52s**; V8's original 18×18 run took **1m 14s**. These earlier timings were measured separately from the latest V8/V9 18×18 comparison.

- [V6 release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v6-19x19)
- [V7 prerelease and instructions](README_V7.md)
- [V5 full catalog, validation and restart release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v5-19x19)

V5 data remains valid and compatible. V2–V4 historical claims are superseded. No 20×20 computation has been performed.
