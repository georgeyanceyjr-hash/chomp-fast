# Fresh V12 10×42 timing

No previous computed positions were loaded. The seed contains only the poisoned single square. The 89 other starting entries are generated from analytic two-row and equal-arm L-shape base cases during the timed run. All 284 nontrivial rectangular classes were computed with 9 workers.

- Fresh run including seed/base generation: **735.729374 seconds (12m 16s)**.
- Solver launch through exit: 735.728534 seconds.
- Padhi published solve: **2141 seconds (35m 41s)**.
- Ratio of reported times: **2.910×**. Different hardware/worker configurations and timing scopes prevent a controlled speed claim.
- Complete catalog: **107,342,199 nonempty P-positions**. Exact Padhi total after subtracting his empty-board entry.
- Winning bites: **(5,36), (7,30), (8,26)**.
- Entire sorted catalog SHA256: `13893537aba7adcf0d5f168d7e9fd9147732c73777021568b18d100051440cb5`; byte-identical to our prior continuation catalog.
- All checkpoint checksums, class sequence, bounds and uniqueness verified. All 420 rectangular opening counts match Padhi through the identical catalog.

Compilation and post-run verification/merging are excluded from both of our reported run timings. Source and solver binary are unchanged from V12. Prior data was used only for comparison after solver exit. No GitHub publication was performed.

Reference: https://github.com/jaideepsaipadhi/chomp-three-winning-moves/blob/d29cd6f6e2a732e986bd40e986f723824352d85b/triple_verify.log
