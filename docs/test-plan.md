# Test plan

Framework: GoogleTest + CTest. No test asserts timing. No exact equality except where arithmetic is exactly representable.

## Tolerance policy
`close(a, b) := |a-b| <= atol + rtol*|b|`, with `atol` and `rtol` chosen per test and justified below. Tolerances are never loosened to make a failure pass without a written numerical reason.

## Cases
| Test | Input | Expectation | Tolerance rationale |
|------|-------|-------------|---------------------|
| Reference x² | x=3 | 6.0 | exact in binary |
| Reference x³, sin, cos, exp | spot values, e.g. cos(0)=1, exp(0)=1 | match known | rtol ~1e-14 (libm accuracy) |
| Forward | x², x=3, h=0.01 | ≈6.01, abs err ≈0.01 | exact algebra gives 6.01; roundoff ~ eps·f/h ≈ 1e-13 → atol 1e-9 |
| Central | x², x=3, h=0.01 | ≈6 | same roundoff bound; truncation error is zero for quadratics |
| Central convergence | sin, x=1, h=1e-1…1e-4 | error decreases roughly ×100 per ×10 drop in h (O(h²)) | assert only in truncation-dominated range; no assertion for tiny h |
| Forward order | sin, x=1, h=1e-1…1e-4 | error decreases roughly ×10 per ×10 | same |
| Invalid h | 0, −1, NaN, +inf | `invalid_step`, no evaluation | — |
| Zero reference | x² at 0 | abs error defined, relative error nullopt | — |
| Domain edge | function with `valid()` false at x±h | `invalid_domain`, no fallback | — |
| Overflow | exp at x=1000 | `non_finite` status visible | — |
| Comparison | 2 methods × 3 h | same (fn,x,h) across methods; stable ordering | — |
| CSV | header, comma/quote escaping, failed path | stable schema v1; error returned | — |
| CLI integration | run binary → CSV | exit 0, schema, key values; bad args/path → nonzero | — |
| Build isolation | Clad/ROOT OFF | baseline passes | — |

Sanitizer (ASan/UBSan) runs complement but do not replace numerical checks. Regression tests are added with every fixed bug.
