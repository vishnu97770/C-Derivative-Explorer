# Experiments

`step-sweep.sh` sweeps h = 1e-1 … 1e-16 (one per decade) for `sin` at x=1 and `exp` at x=0.5 with both methods, writes raw CSVs, plots, and a `manifest.txt` (command, UTC time, commit, compiler, OS) into `experiments/results/`.

```bash
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build/release
experiments/step-sweep.sh            # or pass a path to the executable
```

## What is tracked

`experiments/results/` is tracked (the root-level `/results/` directory is the git-ignored place for ad-hoc output):

| file | origin |
|---|---|
| `step-sweep-sin.csv`, `step-sweep-exp.csv` | raw output of `step-sweep.sh` |
| `step-sweep-sin.png`, `step-sweep-exp.png` | `scripts/plot_results.py` on those CSVs (the README embeds the `sin` plot) |
| `manifest.txt` | command, UTC time, commit, whether the working tree differed from that commit, compiler, OS |
| `bench-raw.csv` | raw per-repetition timings from `derivative-bench` (see `docs/benchmark-methodology.md`); not produced by `step-sweep.sh` |

Re-running `step-sweep.sh` regenerates the CSVs, PNGs and manifest; other compilers or libm versions may differ in the last digits. Rows with a non-ok status are never dropped silently by the plot script; they are counted in the figure footer. In the `sin` sweep the two h = 1e-16 rows are `degenerate_step` (`x + h` rounds to `x`) and are therefore not plotted.
