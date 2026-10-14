# Progress log

## 2026-10-14 — Phase 0 (Inspect) and Phase 1 (planning docs, drafted)
Commands run: `pwd`, `git status --short`, `ls -la`, `git log --oneline`, tool `--version` checks, `df -h .`.

Inventory:
- `/home/hp/Documents/C-Derivative-Explorer`, branch `main`, clean, one commit (`Initial commit`), only a 23-byte README.md.
- cmake 4.2.3, g++ 15.2.0, clang++ 21.1.8, ninja 1.13.2, make 4.4.1, python 3.13.5, matplotlib 3.10.0.
- **Not installed:** clang-format, clang-tidy, GoogleTest (no `/usr/include/gtest`).
- Disk: 21 GB free (91% used).

No code written; nothing built or tested.

## Open decisions needing approval (blueprint §14)
1. Compiler/standard matrix: proposed C++17, GCC + Clang.
2. GoogleTest strategy: proposed pinned `FetchContent` (not installed locally; needs network).
3. Backward difference in first release? Proposed: defer.
4. CLI: flags only, or also config files? Proposed: flags only for MVP; revisit in phase 8.
5. Python/matplotlib plotting as the default analysis path? Proposed: yes.
6. License: not chosen (MIT / Apache-2.0 / other?).
7. Error policy: structured status results (proposed) vs exceptions.
8. Install clang-format/clang-tidy locally (`sudo apt install clang-format clang-tidy`) or rely on CI only?

## 2026-10-15 — Decisions resolved (defaults accepted) and Phases 2–8, 10 (partial)
Decisions taken: C++17 on GCC+Clang; GoogleTest v1.17.0 via FetchContent pinned by SHA-256; backward difference deferred; flags-only CLI (no config files; `experiments/step-sweep.sh` is the reproducible entry); matplotlib plotting; structured status results. Added a `log` function (domain x>0) to exercise domain-edge handling. License: still undecided, no LICENSE file.

Verified (commands run locally):
- Fresh `rm -rf build` configure/build/ctest: GCC 15.2 Debug and Clang 21.1.8 Release → 29/29 pass, zero compiler warnings (-Wall -Wextra -Wpedantic -Wconversion -Wshadow).
- Also passed 28/28 (before the plot test was added): GCC Release, Clang Debug.
- Clang 21.1.8 ASan+UBSan fresh build (fsanitize flags confirmed in build.ninja), all 29 tests pass, no sanitizer reports.
- `clang-format` 23.1.3 (from PyPI, scratchpad venv) applied; dry-run clean.
- Experiment sweep regenerated; plot inspected visually; minima recorded in README.

Not done / not verified:
- GitHub Actions workflow written but **never run**. CI's Ubuntu clang-format may be an older version than 23.1.3 and could report differences.
- clang-tidy: see the later entry below. Phase 11 (Clad), 12 (ROOT) not started. LICENSE missing.
- Phase 13 release checklist not executed; nothing is tagged.

Next action: owner commits and pushes, then check the first CI run; fix any CI-only failures; decide license.

## 2026-10-15 (later) — Phase 9 benchmarks
Implemented `bench/benchmark.cpp` (option `DE_BUILD_BENCHMARKS`, default OFF). Ran once locally (GCC 15.2 Release): 8–16 ns per estimate; raw data in `experiments/results/bench-raw.csv`; table and caveats in docs/benchmark-methodology.md. Not repeated across sessions or compilers. Default build/tests unaffected (option OFF). User clarified they will commit and push only once the whole project is complete; CI therefore remains unrun.

## 2026-10-15 (later) — clang-tidy
clang-tidy 22.1.8 (PyPI, scratchpad venv) run with `.clang-tidy` (bugprone, performance, clang-analyzer; excluding easily-swappable-parameters and enum-size as noise) over src/ and bench/: clean after replacing `atoi` with `strtol` in bench/benchmark.cpp. Added a `tidy` job to CI (unrun on GitHub; apt's clang-tidy version may differ). Tests: 29/29 pass after the change.

Remaining before release: LICENSE (needs owner's choice), Phase 13 checklist, optional Clad/ROOT (not started, deliberately skipped for the core release).

## 2026-10-15 (final) — Phase 13 checks run locally
License: MIT. Clad/ROOT deferred out of 0.1.0. Fresh-copy check (tracked + unignored files only, copied to an empty directory): GCC Release and Clang Debug configure/build/ctest → 29/29 pass, no warnings. README, CHANGELOG, LICENSE present.

Not verified: GitHub Actions (never run), clang-format/clang-tidy versions on Ubuntu runners, other platforms (macOS/Windows), independent-reader review (REQ-014).
Release rule: tag v0.1.0 only after the first CI run on main is green.
Next action: owner commits/pushes, checks CI, then tags.
