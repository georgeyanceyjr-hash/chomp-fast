# Chomp fast

## V11 — complete 20×20 catalog

**451,253,296 unique P-positions**, including every recorded 19×19 position and **304,691,519** additions.

The **19-to-20 continuation took 22 minutes 24 seconds** (1,344.416611708 seconds), using 9 workers. This includes loading the complete 19×19 input and computing all 17 new width-20 classes. It is not a fresh 20×20 timing.

- [V11 release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v11-20x20)
- [FULL 20×20 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v11-20x20/chomp_ppositions_20x20_v11.bin.gz)
- [Complete restart data](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v11-20x20/chomp_v11_20x20_restart.tar.gz)
- [Build, validation, formats and restart instructions](README_V11.md)
- [Manifest](20x20_V11_MANIFEST.json) and [timing report](timing-report-v11-20x20.md)

V11 widens two filter-capacity counters to 64 bits. The exact tested source is preserved. Full checkpoint checksums, ordering, bounds, all new classes and catalog uniqueness passed; the correction also matched the complete 18×18 reference and independent 8×8 continuation results. There is no independent full 20×20 catalog comparison yet.

## Earlier results

V10 completed fresh 19×19 in **385.272641 seconds (6m 25s)**, with the exact V5/V9 checkpoint and 146,561,777 positions. V9 took 387.906746 seconds; V8 took 416.457865 seconds. These were separate measured runs. The historical V9 and V10 release tags retain their original 18×18 labels because publication preceded their full 19×19 runs.

- [V10 documentation](README_V10.md)
- [V9 full 19×19 timing report](timing-report-v9-19x19.md)
- [Original complete 19×19 release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v5-19x19)

All previous releases remain available. V4's uncorrected results are superseded. No 21×21 computation has been started, and no further speed-improvement search is scheduled.
