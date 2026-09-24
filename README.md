# Fast Chomp sieve — V12

V12 supports both square and rectangular computations, preserving V11's move logic and adding rectangular traversal bounds.

The **fresh 10×42 run took 12m 16s with 9 workers**, using no previous computed positions. Its **107,342,199 nonempty P-positions** and winning bites **(5,36), (7,30), (8,26)** agree with Padhi's published results. The complete catalog, source, verification evidence and restart data are available.

- [V12 release and complete 10×42 data](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v12-10x42)
- [V12 build, validation, square/rectangle usage and restart instructions](README_V12.md)
- [Fresh timing report](timing-report-v12-10x42.md)
- [V12 manifest](10x42_V12_MANIFEST.json)
- [V11 release and complete 20×20 data: 451,253,296 P-positions](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/tag/v11-20x20)
- [V11 documentation](README_V11.md)

Padhi's published 10×42 solve took 35m 41s. Our reported time is 2.91 times shorter, but hardware, worker configurations and timing scopes differ. This is an independent reproduction of his result, not a claim to discovering the triple.

V12's square traversal and core logic match V11's; equal square speed is expected but has not been benchmarked head-to-head. Prior releases remain available. Historical V4 results are not validated by the corrected versions.
