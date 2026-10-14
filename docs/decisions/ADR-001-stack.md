# ADR-001: Technology stack

Status: **Accepted 2026-10-15** (defaults approved). Open: license.

| Area | Decision | Alternatives |
|------|----------|--------------|
| Language | C++17 | C++20 (no feature needs it) |
| Compilers | GCC 15.2 and Clang 21.1.8 (both present locally); CI on both | GCC only |
| Build | CMake ≥3.20 + CMakePresets + CTest, Ninja generator | Make |
| Tests | GoogleTest via `FetchContent` pinned to a release tag (not installed system-wide here) | system package, Catch2 |
| Formatting | clang-format with checked-in `.clang-format` (not installed locally; CI-only until installed) | — |
| Static analysis | clang-tidy in CI (not installed locally) | — |
| Sanitizers | ASan+UBSan preset | — |
| Error handling | Structured result/status (see architecture.md) | exceptions, `std::expected` (C++23) |
| CLI | Minimal hand-written flag parser; no config files in MVP | CLI11, JSON config |
| Plotting | Python + matplotlib (3.10.0 present) | ROOT (optional later) |
| Benchmarks | Google Benchmark optional via FetchContent, off by default | hand-rolled harness |
| Optional AD | Clad adapter, default OFF, compatibility verified at phase 11 | — |
| License | Undecided (needs user choice) | MIT / Apache-2.0 |

Consequence: baseline needs only a compiler, CMake and network access for the pinned GoogleTest fetch on first configure.
