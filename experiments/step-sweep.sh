#!/usr/bin/env bash
# Reproduces experiments/results/step-sweep.{csv,png}.
# Usage: experiments/step-sweep.sh [path-to-derivative-explorer]   (default: build/release/...)
set -euo pipefail
cd "$(dirname "$0")/.."
EXE=${1:-build/release/derivative-explorer}
OUT=experiments/results
mkdir -p "$OUT"
# h = 1e-1 ... 1e-16 (one per decade), sin at x=1 and exp at x=0.5, both methods.
"$EXE" --function sin --x 1   --log-steps 1:16 --methods forward,central --csv "$OUT/step-sweep-sin.csv" >/dev/null
"$EXE" --function exp --x 0.5 --log-steps 1:16 --methods forward,central --csv "$OUT/step-sweep-exp.csv" >/dev/null
python3 scripts/plot_results.py "$OUT/step-sweep-sin.csv" "$OUT/step-sweep-sin.png"
python3 scripts/plot_results.py "$OUT/step-sweep-exp.csv" "$OUT/step-sweep-exp.png"
{
  echo "command: experiments/step-sweep.sh"
  echo "date_utc: $(date -u +%FT%TZ)"
  echo "commit: $(git rev-parse HEAD 2>/dev/null || echo unknown)"
  echo "compiler: $(c++ --version | head -1)"
  echo "os: $(uname -sr)"
} > "$OUT/manifest.txt"
