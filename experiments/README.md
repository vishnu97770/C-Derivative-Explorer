# Experiments

`step-sweep.sh` sweeps h = 1e-1 … 1e-16 (one per decade) for `sin` at x=1 and `exp` at x=0.5 with both methods, writes raw CSVs, plots, and a `manifest.txt` (command, UTC time, commit, compiler, OS) into `experiments/results/`.

```bash
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build/release
experiments/step-sweep.sh            # or pass a path to the executable
```

Raw CSVs are committed so plots trace back to data. Re-running reproduces the same numbers on the same toolchain; other compilers/libm may differ in the last digits. Rows with non-ok status are never dropped silently by the plot script; they're counted in the figure footer.
