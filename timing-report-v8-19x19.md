# V8 full 19×19 timing

Fresh run with the original bootstrap seed/base and 9 workers.

- Wall time, solver launch through exit: **416.457865 seconds (6 minutes 56 seconds rounded)**.
- Started: 2026-09-24T15:17:18.985301+00:00
- Finished: 2026-09-24T15:24:15.446938+00:00
- Final checksum checking excluded from solver wall time.
- Full checkpoint SHA256 matches validated V5 exactly: `723271af0730f423cd529f462907feb177f210fcc8ac9c38b01d5070fa99ed06`.
- Complete catalog including seed/base: **146,561,777 unique P-positions** (inherited from identical validated checkpoint and original inputs).
- Terminal class: R(19,18).
- Source SHA256: `6360e13e0d7c08811e587ef5c8e0a6695ca0a28e25ff1183d35dd79a164c0d9a`.
- V6 comparison: 882.522304 seconds; V8 uses 52.81% less time (2.12× as fast).
- V5 comparison: 2273.805030 seconds; V8 uses 81.68% less time (5.46× as fast).

The original checkpoint and log are preserved locally. The result is byte-identical to the full checkpoint in the [existing V5 restart bundle](https://github.com/georgeyanceyjr-hash/chomp-fast/releases/download/v5-19x19/chomp_v5_19x19_restart.tar.gz). No 20×20 computation was started.
