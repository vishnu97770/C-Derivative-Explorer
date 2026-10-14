# Changelog

## [Unreleased] — 0.1.0 candidate
- MIT license; clang-tidy config and CI job.
- Core library: function registry (x2, x3, sin, cos, exp, log), forward/central differences, error metrics, comparison engine, CSV writer.
- CLI `derivative-explorer`; CSV schema v1.
- GoogleTest suite, CLI integration test, plot-script tests; ASan/UBSan option.
- Optional timing harness (`DE_BUILD_BENCHMARKS`) and first measurement.
- Step-size experiment, `scripts/plot_results.py`, GitHub Actions workflow (not yet run on GitHub).
