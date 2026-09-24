# Chomp fast

## V10 — final planned speed-improvement pass

Fresh **18×18 averaged64.743 seconds** across two runs, versus **66.134 seconds** for V9: **2.10% less time**. All four checkpoints match exactly, preserving **37,993,832 unique P-positions**. V10 is validated through 18×18; its full 19×19 runtime is unmeasured.

- [V10 release](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v10-18x18)
- [Build, validation and data links](README_V10.md)
- [Measured V10 report](speed-report-v10.md)
- [V10 manifest](18x18_V10_MANIFEST.json)

## Latest full 19×19 validation: V9

V9 completed fresh **19×19 in 387.906746 seconds (6m28s)** with 9 workers, versus the prior V8 run of 416.457865 seconds: **6.86% less time**. The complete checkpoint matches validated V5 and V8, confirming **146,561,777 unique P-positions**, including original seed and base. The historical V9 release retains its 18×18 label because publication preceded that full 19 run.

- [V9 full 19 timing and exact checkpoint hash](timing-report-v9-19x19.md)
- [Exact tested V9 source](chomp_sieve_v9_candidate.cpp)
- [FULL 19×19 catalog](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_ppositions_19x19_v5.bin.gz)
- [Complete original seed/base and checkpoint restart bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz)
- [Tested future 20×20 continuation instructions](README_V6.md)

Earlier releases remain available. V5 corrected the boundary-move 32-bit popcount bug; uncorrected V4 results are not validated. No 20×20 computation has been started.
