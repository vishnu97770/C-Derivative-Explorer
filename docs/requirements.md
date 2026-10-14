# Requirements

Source: *C++ Derivative Explorer Implementation Blueprint* v1.0 (14 Oct 2026), section 13.
Status: **planning only — nothing below is implemented or verified.**

## Functional
| ID | Requirement | Phase | Acceptance test (planned) |
|----|-------------|-------|---------------------------|
| REQ-001 | Baseline builds with documented C++ standard (C++17) and compiler (GCC, Clang) | 2 | Fresh configure/build, Debug+Release, both compilers |
| REQ-002 | Functions x², x³, sin, cos, exp have analytical reference derivatives; radians and domains documented | 3 | `test_functions.cpp`: known values (e.g. x² at 3 → 6.0) |
| REQ-003 | Forward and central differences implemented correctly | 4 | `test_methods.cpp`: x², x=3, h=0.01 → ~6.01 / ~6 within justified tolerance |
| REQ-004 | h validated (zero, negative, NaN, ±inf rejected before calculation) | 4 | `test_methods.cpp` invalid-h cases |
| REQ-005 | Error metrics handle zero/near-zero reference safely | 5 | `test_metrics.cpp`: x² at 0 → abs error defined, relative error not-applicable |
| REQ-006 | All methods use identical (function, x, h) configurations | 6 | `test_comparison.cpp` inspects records |
| REQ-007 | CSV schema documented and stable (schema_version=1) | 7 | `test_csv_writer.cpp`: header, escaping, sample file |
| REQ-008 | Invalid-domain / non-finite results visible via status, never reported as ok | 5–7 | Status tests (exp overflow, domain edge, bad output path → nonzero exit) |
| REQ-009 | Experiments reproducible (config, script, manifest, raw output policy) | 8 | Re-run documented sweep, diff against committed CSV |
| REQ-010 | Benchmarks defensible (methodology, repeats, environment, raw data) | 9 | docs/benchmark-methodology.md followed; no timing asserts in tests |
| REQ-011 | CI builds and tests on supported matrix | 10 | Green GitHub Actions run |
| REQ-012 | Optional Clad never affects baseline | 11 | Build with flag OFF |
| REQ-013 | Optional ROOT never affects baseline | 12 | Build with flag OFF |
| REQ-014 | Docs allow independent use | 13 | Fresh-checkout walkthrough |
| REQ-015 | Release claims match verified evidence | 13 | Release checklist vs docs/progress.md |

## Non-functional
- Pure numerics separated from CLI/IO/CSV (blueprint §5).
- Double precision; floating-point results documented as approximate.
- No silent fallback between methods; no silent alteration of h.
- Baseline has no optional dependencies beyond the chosen test framework.

## Out of scope
Web UI, database, cloud deployment, custom symbolic/AD engine, expression parsing of user input, broad numerical-library features. Clad, ROOT, backward difference, higher-order stencils and multivariable derivatives are optional extensions (post-core).
