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

## Implementation and first measurement (2026-10-15)
Harness: `bench/benchmark.cpp`, built with `-DDE_BUILD_BENCHMARKS=ON` (default OFF, no extra dependency). Run: `build/release/derivative-bench 200000 15 experiments/results/bench-raw.csv`. Workload id `estimate_throughput_v1`: 200 000 points x in [0.1, 2.1), h = 1e-5, one estimate per point per method; 3 warm-up + 15 timed repetitions; steady_clock; per-point ns = rep time / N; median and quartiles reported; raw per-rep times and checksums in `bench-raw.csv`. Setup is outside the timed region. The checksum is consumed through a volatile sink; the failure count is reported (0 here).

Environment: GCC 15.2.0, CMake Release (default flags), Linux 7.0.0-38, 12th Gen Intel Core i5-12450H (12 logical CPUs, frequency scaling active, governor not pinned), load average ≈0.1, laptop, not isolated.

| function | method | median ns/point | Q1 | Q3 |
|---|---|---:|---:|---:|
| x2 | forward | 8.13 | 7.90 | 8.41 |
| x2 | central | 8.55 | 8.27 | 8.73 |
| sin | forward | 15.07 | 14.87 | 15.53 |
| sin | central | 15.97 | 15.82 | 16.07 |
| exp | forward | 14.77 | 14.73 | 14.89 |
| exp | central | 15.19 | 15.11 | 15.26 |

Interpretation limits: both methods evaluate f twice (forward also validates the domain of x), so the small gap is expected and says nothing general. The timed path includes `std::function` indirection and domain checks, which dominate for cheap functions like x². Results are specific to this machine and run; single session, not repeated across days or cores; no cross-compiler comparison.
