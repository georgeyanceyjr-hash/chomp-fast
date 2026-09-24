# V9 full 19×19 timing

Fresh V9 run using the original seed and dimension-19 base, 9 workers.

- V9 wall time: **387.906746 seconds (6 minutes 28 seconds rounded)**.
- Previous V8 wall time: **416.457865 seconds**; V8 was not rerun for this comparison.
- Time saved: **28.551118 seconds (6.86% less time)**.
- Timing includes launch, load, solving, checkpoint writes and solver exit; final hashing excluded.
- Started: 2026-09-24T16:08:58.704160+00:00
- Finished: 2026-09-24T16:15:26.607007+00:00
- Full checkpoint SHA256: `723271af0730f423cd529f462907feb177f210fcc8ac9c38b01d5070fa99ed06` — identical to validated V5 and V8.
- Full catalog including original seed and base: **146,561,777 unique P-positions**.
- Checkpoint: 136 records, terminal class R(19,18).
- Tested source SHA256: `6b42adb8956f1ee107ea80b4ddcf38428fc20de389894deb7c1635f67cb7b8ab`.

This is a single-machine comparison with V8's earlier recorded run, not an average or simultaneous benchmark. Source was published before this run. The original outputs are preserved locally. The complete checkpoint is also available in the existing V5 restart bundle linked in README_V10.md. No 20×20 computation was started.
