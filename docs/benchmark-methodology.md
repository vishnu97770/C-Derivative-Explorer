# Benchmark methodology

Status: plan only; no measurements exist.

**Question:** runtime cost of one derivative estimate (forward vs central) for a fixed function set, and throughput over a fixed set of N points.

- Release build; record compiler+version, flags, OS, CPU, load, commit hash, timestamp.
- Equivalent work per method (forward = 2 evaluations, central = 2 evaluations; report this).
- Keep results alive (e.g. `benchmark::DoNotOptimize` or accumulate into a checksum) so the optimizer cannot remove work.
- Exclude setup and file I/O from the timed region.
- Warm-up, then ≥ 10 repetitions; report median and IQR; keep raw trial times.
- Correctness and error recorded alongside timing, outside the timed region.
- Record schema per blueprint §10. Timing thresholds are never used as test conditions.
- Analytical reference is not benchmarked as a competing algorithm. Any Clad comparison states its measurement boundary (setup vs run).
