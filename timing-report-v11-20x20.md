# V11 complete20×20 result

**451,253,296 unique P-positions**, an increase of **304,691,519** beyond19×19.

The19-to20 continuation took **1,344.416611708 seconds (22 minutes24 seconds rounded)** using9 workers. It started at2026-09-24T16:56:29.589054Z and exited at2026-09-24T17:18:53.991379Z. Time includes loading the full19×19 preload, solving, checkpoint writes and exit; it excludes input preparation and the15.586965 seconds used for final validation, merging and hashing. This is a continuation measurement, not a fresh20×20 run or a speed comparison with a different starting dataset.

The17 new classes run from R(20,3) through R(20,19). The final class contributed56,493,626 positions before transposes, or112,987,252 including both orientations.

The full disjoint catalog contains102 seed positions,34 dimension20 base cases,146,561,644 historical19 checkpoint positions and304,691,516 new width20 checkpoint positions. Three of the34 base cases are new beyond dimension19.

- Full catalog:3,610,026,400 bytes; SHA256 `b2d8e2bda7c158162fc61f14f294d5deeba0acbe96c1fd62ba1e765924a88ec4`.
- New width20 checkpoint:2,437,532,544 bytes; SHA256 `7b94bb92f3474bc70014b176a117420d473a2985e77d1586c72ef9f1af6c16c3`.
- Exact tested source: `5ce78972e5d9f6382fa870a516624e12560db00b030b9d7df2528973d8993244`.

All checkpoint checksums, strict per-record ordering, dimension bounds, class sequence and lack of overlapping catalog entries passed. The complete sorted64-bit catalog is supplied, not a sample. Capacity widening matched the full18×18 reference; a7-to8 continuation matched all322 independently solved8×8 P-positions. No independent full20 catalog comparison has been performed.

This release is V11. During execution it was called V10 capacity64; the user assigned V11 while the run was active. Published source and catalog bytes are unchanged, and the manifest maps the old names to the release names.
