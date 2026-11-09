#!/usr/bin/env python3
"""ESPA-483 active-sensing round: same PIR-based scoring as
espa483_relabel_pir.py (round 2), run against the active-TX (csi_active_pps=50)
capture instead of the passive one, plus the fps-under-active-TX comparison
and a three-way table against round 2's passive numbers (hardcoded below --
those came from a report already posted on the issue, not recomputed here).
"""
import argparse
import json
import sys
from datetime import datetime, timedelta, timezone

import numpy as np

sys.path.insert(0, ".")
from espa483_analyze import WINDOW_SECONDS
from espa483_relabel_pir import (load_pir_events, relabel_room, evaluate,
                                  baseline_evaluate, plot_room, MOTION_RADIUS_S,
                                  EMPTY_MARGIN_S)

# From the round-2 report already posted on ESPA-483 (passive, csi_active_pps=0).
PASSIVE_REFERENCE = {
    "family": {"espectre_auc": 0.619, "baseline_auc": 0.712, "espectre_recall": 0.16, "baseline_recall": 0.31},
    "laundry": {"espectre_auc": 0.633, "baseline_auc": 0.692, "espectre_recall": 0.24, "baseline_recall": 0.33},
}


def night_mask(centers, start_et=1, end_et=6, year=2026, month=11, day=9):
    """01:00-06:00 America/New_York (EST, UTC-5) on the given date = +5h UTC."""
    lo = datetime(year, month, day, start_et + 5, 0, tzinfo=timezone.utc)
    hi = datetime(year, month, day, end_et + 5, 0, tzinfo=timezone.utc)
    return np.array([lo <= c < hi for c in centers])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--report-json", required=True)
    ap.add_argument("--family-pir", required=True)
    ap.add_argument("--laundry-pir", required=True)
    ap.add_argument("--out-md", required=True)
    ap.add_argument("--out-dir", required=True)
    ap.add_argument("--capture-hours", type=float, default=24.1)
    args = ap.parse_args()

    with open(args.report_json) as f:
        data = json.load(f)

    rooms = {
        "family": load_pir_events(args.family_pir),
        "laundry": load_pir_events(args.laundry_pir),
    }

    lines = ["# ESPA-483 active-sensing (csi_active_pps=50): scored against PIR motion sensors\n\n"]
    lines.append(f"Same labelling as round 2: window center within ±{MOTION_RADIUS_S}s of a PIR 'on' = motion; "
                  f"more than {EMPTY_MARGIN_S}s from any PIR 'on' = empty; the band in between excluded.\n\n")

    summary_rows = []
    for room, on_times in rooms.items():
        r = data["rooms"].get(room)
        if r is None:
            lines.append(f"## {room}\nNo cached data for this room.\n\n")
            continue
        X, y, centers, n_kept, n_total = relabel_room(r, on_times)
        fps = r["n_frames"] / (args.capture_hours * 3600.0)
        lines.append(f"## {room}\n")
        lines.append(f"- {r['n_frames']} CSICAP frames over ~{args.capture_hours:.1f}h -> "
                      f"**{fps:.1f} fps average under active TX** (passive rounds measured 7-23 fps).\n")
        lines.append(f"- {n_kept}/{n_total} windows scored ({n_total - n_kept} excluded as PIR hold-time), "
                      f"{int(y.sum())} motion, {int((~y).sum())} empty.\n")
        if len(X) < 10 or y.sum() < 5 or (~y).sum() < 5:
            lines.append("- Not enough labelled variety to fit after excluding the hold-time band.\n\n")
            continue

        res = evaluate(X, y)
        lines.append(f"- **Active ESpectre features (class-balanced, 5%-FA-calibrated):** AUC={res['auc']:.3f}, "
                      f"threshold={res['threshold']:.2f}: recall={res['recall']:.2f}, "
                      f"empty-room FA/hr={res['fa_per_hour']:.2f} "
                      f"(tp={res['tp']} fn={res['fn']} fp={res['fp']} tn={res['tn']})\n")

        bl = baseline_evaluate(r["baseline_variances"], centers, y)
        if bl:
            lines.append(f"- **Active csi_variance+percentile baseline** (kind `{bl['dominant_kind']}`, "
                          f"floor={bl['floor']:.1f}): AUC={bl['auc']:.3f}, recall={bl['recall']:.2f}, "
                          f"empty-room FA/hr={bl['fa_per_hour']:.2f} "
                          f"(tp={bl['tp']} fn={bl['fn']} fp={bl['fp']} tn={bl['tn']})\n")
        else:
            lines.append("- **Active csi_variance+percentile baseline:** not enough samples to score.\n")
            bl = None

        night = night_mask(centers)
        night_idx = np.where(night)[0]
        if len(night_idx) > 5:
            night_probs = res["probs"][night_idx]
            night_labels = y[night_idx]
            night_empty_only = night_probs[~night_labels]
            if len(night_empty_only) > 5:
                night_fp_rate = float(np.mean(night_empty_only >= res["threshold"]))
                night_fa_per_hour = night_fp_rate * (3600.0 / WINDOW_SECONDS)
                lines.append(f"- **Night window (01:00-06:00 ET, truly-empty check):** "
                              f"{len(night_empty_only)} empty-labelled windows, "
                              f"false-alarm rate {night_fp_rate:.3f} → {night_fa_per_hour:.1f} FA/hr at the same threshold.\n")
            else:
                lines.append("- **Night window:** not enough empty-labelled windows overnight to score.\n")
        else:
            lines.append("- **Night window:** not enough windows in 01:00-06:00 ET to score.\n")

        plot_path = f"{args.out_dir}/{room}_active_score_vs_pir.png"
        plot_room(room, centers, res["probs"], y, on_times, plot_path)
        lines.append(f"- Plot: `{plot_path}`\n\n")

        summary_rows.append({
            "room": room, "fps": fps,
            "active_espectre_auc": res["auc"], "active_espectre_recall": res["recall"],
            "active_baseline_auc": bl["auc"] if bl else float("nan"),
            "active_baseline_recall": bl["recall"] if bl else float("nan"),
        })

    lines.append("## Three-way comparison: active ESpectre feats vs active baseline vs existing passive\n\n")
    lines.append("| room | fps (active) | AUC active ESpectre | AUC active baseline | AUC passive ESpectre (round 2) | AUC passive baseline (round 2) |\n")
    lines.append("|---|---|---|---|---|---|\n")
    for row in summary_rows:
        ref = PASSIVE_REFERENCE.get(row["room"], {})
        lines.append(f"| {row['room']} | {row['fps']:.1f} | {row['active_espectre_auc']:.3f} | "
                      f"{row['active_baseline_auc']:.3f} | {ref.get('espectre_auc', float('nan')):.3f} | "
                      f"{ref.get('baseline_auc', float('nan')):.3f} |\n")

    with open(args.out_md, "w") as f:
        f.writelines(lines)
    print("".join(lines))


if __name__ == "__main__":
    main()
