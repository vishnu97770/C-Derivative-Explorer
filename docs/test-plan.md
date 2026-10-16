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
| Degenerate stencil | sin at x=1, h=1e-17, 1e-16, eps/2 (forward); 1e-17, 1e-16 (central) | `degenerate_step`, NaN estimate | exact: ulp(1)=eps=2.2e-16, ties round to even |
| One-sided rounding | central at x=1, h=1e-16 (x+h==1, x-h!=1) | `degenerate_step` (was a bogus ok) | exact |
| Smallest usable step | forward/central at x=1, h=eps | ok | exact |
| Degenerate vs magnitude | x2 at x=1e16, h=1 vs x=1, h=1 | degenerate vs ok | ulp(1e16)=2 |
| Status precedence | log at x=-1, h=1e-20; h=0; x=inf | invalid_domain / invalid_step / invalid_domain | — |
| Exit codes | partial success; all rows overflow/degenerate; degenerate+ok; csv failure (also with all-non-ok); malformed arg | 0; 3; 0; 1; 2 | — |
| --log-steps count | 1.5, 0, -3, abc, empty, +3, 1e2, overflow, 100001, extra colon, B<A, nan, huge range, missing B | exit 2; valid 1:3, 1:3:5, 2:2:1 give 3, 5, 1 steps | — |
| Build isolation | Clad/ROOT OFF | baseline passes | — |

Sanitizer (ASan/UBSan) runs complement but do not replace numerical checks. Regression tests are added with every fixed bug.
