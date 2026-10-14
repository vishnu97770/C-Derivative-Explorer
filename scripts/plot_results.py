#!/usr/bin/env python3
"""Plot |error| vs h (log-log) from derivative-explorer CSV (schema_version 1).

Usage: plot_results.py INPUT.csv OUTPUT.png
One panel per (function, x); one line per method. Rows with non-ok status are not
plotted but are counted and listed in the figure footer, never silently dropped.
"""
import csv
import sys
from collections import defaultdict

REQUIRED = ["schema_version", "function_id", "method_id", "x", "h", "absolute_error", "status"]


def load(path):
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        missing = [c for c in REQUIRED if c not in (reader.fieldnames or [])]
        if missing:
            raise SystemExit(f"error: {path}: missing columns {missing}")
        rows = list(reader)
    if not rows:
        raise SystemExit(f"error: {path}: no data rows")
    bad = {r["schema_version"] for r in rows} - {"1"}
    if bad:
        raise SystemExit(f"error: {path}: unsupported schema_version {sorted(bad)}")
    return rows


def main(argv):
    if len(argv) != 3:
        raise SystemExit(__doc__)
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    rows = load(argv[1])
    panels = defaultdict(lambda: defaultdict(list))
    skipped = defaultdict(int)
    for r in rows:
        key = (r["function_id"], float(r["x"]))
        if r["status"] != "ok" or r["absolute_error"] == "":
            skipped[r["status"]] += 1
            continue
        panels[key][r["method_id"]].append((float(r["h"]), float(r["absolute_error"])))

    if not panels:
        raise SystemExit("error: no plottable (status=ok) rows")
    n = len(panels)
    fig, axes = plt.subplots(1, n, figsize=(5.5 * n, 4.5), squeeze=False)
    for ax, ((fn, x), methods) in zip(axes[0], sorted(panels.items())):
        for m, pts in sorted(methods.items()):
            pts.sort()
            # error == 0 cannot be drawn on a log axis; clamp for display only
            ax.loglog([p[0] for p in pts], [max(p[1], 1e-18) for p in pts], "o-", label=m, ms=4)
        ax.set_title(f"{fn}'(x) at x={x:g}")
        ax.set_xlabel("step size h")
        ax.set_ylabel("absolute error")
        ax.grid(True, which="both", alpha=0.3)
        ax.legend()
    note = f"source: {argv[1]}"
    if skipped:
        note += "   not plotted: " + ", ".join(f"{k}={v}" for k, v in sorted(skipped.items()))
    fig.text(0.01, 0.005, note + "   (zero error clamped to 1e-18 for display)", fontsize=7)
    fig.tight_layout(rect=(0, 0.03, 1, 1))
    fig.savefig(argv[2], dpi=140)
    print(f"wrote {argv[2]} ({n} panels, skipped: {dict(skipped) or 'none'})")


if __name__ == "__main__":
    main(sys.argv)
