#!/usr/bin/env python3
"""
ESPA-483 step 2, round 2: re-score against PIR motion sensors instead of the
presence/group sensors used in round 1 (DT's finding: family_presence_occupancy
is mmWave presence, not motion, and office's sensors are dead). Reuses the
already-computed per-window features (autocorr, iqr_over_mean_aggr) and
baseline variances from espa483_analyze.py's cached JSON -- no need to re-parse
the raw capture, only the labels change.

Labelling: a window is MOTION if its center falls within +/-60s of any PIR
"on" transition, EMPTY if it's more than 5min from every PIR "on" transition,
and EXCLUDED otherwise (the PIR's own hold/cooldown band, which would punish
a correct detector for not re-firing while the PIR is still latched on).
"""
import argparse
import json
import sys
from datetime import datetime, timedelta, timezone

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

sys.path.insert(0, ".")
from espa483_analyze import (fit_logistic, predict_logistic, auc_roc,
                              calibrate_threshold_from_empty, WINDOW_SECONDS)

MOTION_RADIUS_S = 60
EMPTY_MARGIN_S = 300


def load_pir_events(path):
    with open(path) as f:
        doc = json.load(f)
    events = []
    for entity in doc["data"]["entities"]:
        for s in entity["states"]:
            ts = datetime.fromisoformat(s["last_changed"]).astimezone(timezone.utc)
            events.append((ts, s["state"]))
    events.sort(key=lambda r: r[0])
    on_times = [ts for ts, state in events if state == "on"]
    return on_times


def label_window(center, on_times, motion_radius=MOTION_RADIUS_S, empty_margin=EMPTY_MARGIN_S):
    if not on_times:
        return 0  # no PIR events at all in this capture -> treat as empty
    nearest = min(abs((center - t).total_seconds()) for t in on_times)
    if nearest <= motion_radius:
        return 1
    if nearest > empty_margin:
        return 0
    return None  # excluded: inside the PIR's own hold/cooldown band


def relabel_room(report_room, on_times):
    starts = [datetime.fromisoformat(s) for s in report_room["starts5"]]
    X = np.array(report_room["X5"])
    centers = [s + timedelta(seconds=WINDOW_SECONDS / 2) for s in starts]
    labels = [label_window(c, on_times) for c in centers]
    keep = [i for i, l in enumerate(labels) if l is not None]
    X_kept = X[keep]
    y_kept = np.array([labels[i] for i in keep], dtype=bool)
    centers_kept = [centers[i] for i in keep]
    return X_kept, y_kept, centers_kept, len(keep), len(labels)


def night_mask(centers, start_et=1, end_et=6, day=6):
    """2026-11-06 01:00-06:00 America/New_York (EST, UTC-5) = 06:00-11:00 UTC."""
    lo = datetime(2026, 11, 6, start_et + 5, 0, tzinfo=timezone.utc)
    hi = datetime(2026, 11, 6, end_et + 5, 0, tzinfo=timezone.utc)
    return np.array([lo <= c < hi for c in centers])


def evaluate(X, y, target_fa_rate=0.05):
    y = np.asarray(y, dtype=bool)
    model = fit_logistic(X, y)
    probs = predict_logistic(model, X)
    auc = auc_roc(probs, y)
    threshold = calibrate_threshold_from_empty(probs, y, target_fa_rate)
    pred = probs >= threshold
    tp = int(np.sum(pred & y))
    fn = int(np.sum((~pred) & y))
    fp = int(np.sum(pred & (~y)))
    tn = int(np.sum((~pred) & (~y)))
    n_empty = fp + tn
    hours = n_empty * WINDOW_SECONDS / 3600.0
    fa_per_hour = fp / hours if hours > 0 else float("nan")
    recall = tp / max(tp + fn, 1) if (tp + fn) > 0 else float("nan")
    return {"model": model, "probs": probs, "pred": pred, "auc": auc, "threshold": threshold,
            "tp": tp, "fn": fn, "fp": fp, "tn": tn, "recall": recall, "fa_per_hour": fa_per_hour,
            "n_occupied": tp + fn, "n_empty": n_empty}


def baseline_evaluate(baseline_variances, centers, y, target_percentile=90):
    import bisect
    bl = [(datetime.fromisoformat(w), k, v) for w, k, v in baseline_variances]
    bl.sort(key=lambda r: r[0])
    from collections import defaultdict
    counts = defaultdict(int)
    for _, k, _ in bl:
        counts[k] += 1
    if not counts:
        return None
    dominant = max(counts, key=counts.get)
    series = sorted([(t, v) for t, k, v in bl if k == dominant], key=lambda r: r[0])
    if len(series) < 5:
        return None
    times = [t for t, v in series]
    values = np.array([v for t, v in series])

    def nearest(ts):
        idx = bisect.bisect_left(times, ts)
        if idx == 0:
            return values[0]
        if idx >= len(times):
            return values[-1]
        before, after = times[idx - 1], times[idx]
        return values[idx - 1] if (ts - before) <= (after - ts) else values[idx]

    readings = np.array([nearest(c) for c in centers])
    y = np.asarray(y, dtype=bool)
    empty_readings = readings[~y]
    if len(empty_readings) < 5:
        return None
    floor = float(np.percentile(empty_readings, target_percentile))
    pred = readings > floor
    tp = int(np.sum(pred & y)); fn = int(np.sum((~pred) & y))
    fp = int(np.sum(pred & (~y))); tn = int(np.sum((~pred) & (~y)))
    hours = (fp + tn) * WINDOW_SECONDS / 3600.0
    return {
        "dominant_kind": dominant, "floor": floor, "readings": readings,
        "auc": auc_roc(readings, y),
        "recall": tp / max(tp + fn, 1) if (tp + fn) else float("nan"),
        "fa_per_hour": fp / hours if hours > 0 else float("nan"),
        "tp": tp, "fn": fn, "fp": fp, "tn": tn,
    }


def plot_room(room, centers, probs, y_true, on_times, out_path, hours=6):
    if not centers:
        return
    t0 = centers[0]
    t_end = t0 + timedelta(hours=hours)
    idx = [i for i, c in enumerate(centers) if t0 <= c < t_end]
    if len(idx) < 5:
        idx = list(range(min(len(centers), 500)))
    ts = [centers[i] for i in idx]
    ps = [probs[i] for i in idx]
    fig, ax = plt.subplots(figsize=(12, 3.5))
    for on_t in on_times:
        if ts and ts[0] - timedelta(seconds=MOTION_RADIUS_S) <= on_t <= ts[-1] + timedelta(seconds=MOTION_RADIUS_S):
            ax.axvspan(on_t - timedelta(seconds=MOTION_RADIUS_S), on_t + timedelta(seconds=MOTION_RADIUS_S),
                       color="tab:orange", alpha=0.3, lw=0)
    ax.plot(ts, ps, color="tab:blue", lw=0.8)
    ax.set_ylim(-0.02, 1.02)
    ax.set_ylabel("motion probability")
    ax.set_title(f"{room}: ESpectre-feature score vs PIR motion (orange = PIR on ±{MOTION_RADIUS_S}s)")
    fig.autofmt_xdate()
    fig.tight_layout()
    fig.savefig(out_path, dpi=110)
    plt.close(fig)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--report-json", required=True)
    ap.add_argument("--family-pir", required=True)
    ap.add_argument("--laundry-pir", required=True)
    ap.add_argument("--out-md", required=True)
    ap.add_argument("--out-dir", required=True)
    args = ap.parse_args()

    with open(args.report_json) as f:
        data = json.load(f)

    rooms = {
        "family": load_pir_events(args.family_pir),
        "laundry": load_pir_events(args.laundry_pir),
    }

    lines = ["# ESPA-483 step 2, round 2: re-scored against PIR motion sensors\n\n"]
    lines.append(f"Labelling: window center within ±{MOTION_RADIUS_S}s of a PIR 'on' = motion; "
                  f"more than {EMPTY_MARGIN_S}s from any PIR 'on' = empty; the band in between excluded "
                  f"(the PIR's own hold time, not scored either way).\n\n")

    for room, on_times in rooms.items():
        r = data["rooms"].get(room)
        if r is None:
            lines.append(f"## {room}\nNo cached data for this room.\n\n")
            continue
        X, y, centers, n_kept, n_total = relabel_room(r, on_times)
        lines.append(f"## {room}\n")
        lines.append(f"- {n_kept}/{n_total} windows scored ({n_total - n_kept} excluded as PIR hold-time), "
                      f"{int(y.sum())} motion, {int((~y).sum())} empty.\n")
        if len(X) < 10 or y.sum() < 5 or (~y).sum() < 5:
            lines.append("- Not enough labelled variety to fit after excluding the hold-time band.\n\n")
            continue

        res = evaluate(X, y)
        lines.append(f"- **ESpectre features (class-balanced, 5%-FA-calibrated):** AUC={res['auc']:.3f}, "
                      f"threshold={res['threshold']:.2f}: recall={res['recall']:.2f}, "
                      f"empty-room FA/hr={res['fa_per_hour']:.2f} "
                      f"(tp={res['tp']} fn={res['fn']} fp={res['fp']} tn={res['tn']})\n")

        bl = baseline_evaluate(r["baseline_variances"], centers, y)
        if bl:
            lines.append(f"- **Existing csi_variance+percentile baseline** (kind `{bl['dominant_kind']}`, "
                          f"floor={bl['floor']:.1f}): AUC={bl['auc']:.3f}, recall={bl['recall']:.2f}, "
                          f"empty-room FA/hr={bl['fa_per_hour']:.2f} "
                          f"(tp={bl['tp']} fn={bl['fn']} fp={bl['fp']} tn={bl['tn']})\n")
        else:
            lines.append("- **Existing csi_variance+percentile baseline:** not enough samples to score.\n")

        night = night_mask(centers)
        # night FA-rate: among this room's night-window-and-labelled-empty windows, what
        # fraction does the already-fit model (same threshold) flag as motion?
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

        plot_path = f"{args.out_dir}/{room}_score_vs_pir.png"
        plot_room(room, centers, res["probs"], y, on_times, plot_path)
        lines.append(f"- Plot: `{plot_path}`\n")
        lines.append("\n")

    with open(args.out_md, "w") as f:
        f.writelines(lines)
    print("".join(lines))


if __name__ == "__main__":
    main()
