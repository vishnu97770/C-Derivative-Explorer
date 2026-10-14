# Implementation plan

Each phase ends with a go/no-go gate; no phase starts before the previous gate passes and its evidence is logged in `progress.md`. One commit (or small PR) per phase.

| Phase | Deliverable | Depends on | Gate |
|-------|-------------|-----------|------|
| 0 | Inventory (done, see progress.md) | — | Inventory written |
| 1 | requirements, architecture, ADR-001, plan, test-plan, benchmark methodology | 0 | User approves open decisions |
| 2 | CMakeLists, presets, .gitignore, .clang-format, trivial main + smoke test | 1 | Debug+Release configure/build/ctest pass on GCC and Clang |
| 3 | Function library + `test_functions.cpp` | 2 | REQ-002 tests pass |
| 4 | Forward/central + `test_methods.cpp` | 3 | REQ-003/004 pass |
| 5 | Result/status/metrics + `test_metrics.cpp` | 4 | REQ-005/008 pass |
| 6 | Comparison engine + tests | 5 | REQ-006 pass |
| 7 | CLI + CSV writer + tests (incl. integration, bad path) | 6 | REQ-007/008 pass; example command runs |
| 8 | Experiment config, `plot_results.py`, experiments README | 7 | REQ-009: plots regenerate from CSV |
| 9 | Benchmarks | 7 | REQ-010 methodology followed |
| 10 | Warnings, format check, tidy, sanitizers, GitHub Actions | 2–7 | REQ-011 green run |
| 11 | Optional Clad | 10 | REQ-012 |
| 12 | Optional ROOT | 8 | REQ-013 |
| 13 | README, LICENSE, CHANGELOG, tag | all | REQ-014/015 checklist |

Phases 9, 11, 12 are skippable without blocking a core release.
