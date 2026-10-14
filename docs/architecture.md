# Architecture

Layers (blueprint §5), each depending only on those above it in the list:

1. **Function model** (`function.hpp/.cpp`) — id, value, reference derivative, explicit domain predicate. No I/O.
2. **Derivative methods** (`derivative_methods.hpp/.cpp`) — forward, central (backward deferred). Validates h, evaluates only its stencil points. No printing, no CSV.
3. **Result/metrics** (`result.hpp`, `metrics.hpp/.cpp`) — status enum, absolute error, relative error as `std::optional<double>`.
4. **Comparison engine** (`comparison.hpp/.cpp`) — loops methods over identical (function, x, h), collects `DerivativeResult` records.
5. **CSV writer** (`csv_writer.hpp/.cpp`) — serialises records, escapes fields, checks stream errors, no recomputation.
6. **CLI** (`main.cpp`) — flag parsing, user-input validation, exit codes.
7. **Optional adapters** (`optional/clad`, `optional/root`) — behind CMake flags, default OFF.

## Error policy (proposed — needs approval, see ADR-001)
Batch experiments need one bad configuration not to abort the run, so methods return a **structured result** (value + status), not exceptions:
`ok | invalid_step | invalid_domain | non_finite`.
Exceptions are reserved for programmer errors / unexpected internal failures, which fail loudly with a nonzero exit.
Relative error is `nullopt` when |reference| is zero or below a documented threshold; CSV writes an empty field plus status.
NaN/inf are never written as numbers; the row carries a non-ok status and empty numeric fields.

## Data flow
CLI → function registry → comparison engine (methods × steps) → metrics → records → terminal summary and/or CSV → `scripts/plot_results.py`.

## Domain policy
`Function` carries a `valid(x)` predicate. A stencil is valid only if all its sample points are valid; otherwise the record gets `invalid_domain`. No fallback to another method.
