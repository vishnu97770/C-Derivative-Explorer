# Changelog

## [Unreleased] — 0.1.0 candidate
- New status `degenerate_step` for stencils whose points are not distinct from x (fixes false `ok` at h=1e-16 for sin at x=1, including central one-sided rounding).
- CLI exits with code 3 when every result is non-ok; `--log-steps` count must be an integer in 1..100000 (fractional values were previously truncated silently).
- `experiments/results/` is now tracked (the ignore rule `results/` was swallowing it); manifest records whether the tree was modified.
- MIT license; clang-tidy config and CI job.
- Core library: function registry (x2, x3, sin, cos, exp, log), forward/central differences, error metrics, comparison engine, CSV writer.
- CLI `derivative-explorer`; CSV schema v1.
- GoogleTest suite, CLI integration test, plot-script tests; ASan/UBSan option.
- Optional timing harness (`DE_BUILD_BENCHMARKS`) and first measurement.
- Step-size experiment, `scripts/plot_results.py`, GitHub Actions workflow (not yet run on GitHub).
