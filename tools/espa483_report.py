#!/usr/bin/env python3
"""Turn espa483_analyze.py's JSON dump into the metrics table ESPA-483 asks for:
per-room and pooled recall / empty-room false-alarms-per-hour / detection latency
for the ESpectre-style detector (fit on our data) vs. the existing csi_variance +
percentile baseline, at both the 5s window and the 20pps-grid admission.
"""
import argparse
import json
import sys
import numpy as np
from collections import defaultdict

sys.path.insert(0, ".")
from espa483_analyze import (fit_logistic, predict_logistic, auc_roc,
                              calibrate_threshold_from_empty, WINDOW_SECONDS)


def evaluate(X, y, model=None, target_fa_rate=0.05):
    """Fits (class-balanced) if no model given, reports AUC (the actual
    separation question, threshold-independent) plus recall/FA-rate at a
    threshold calibrated from this room's own empty-labelled windows to a
    target 5% false-alarm rate -- matching how ESpectre's own design
    calibrates (from quiet-room data, not a mixed-label metric like F1, which
    degenerates to an always-positive threshold on a majority-occupied room).
    This is an in-sample calibration (same data informs the threshold and the
    score), an offline feasibility check, not a held-out validation."""
    y = np.asarray(y, dtype=bool)
    if model is None:
        model = fit_logistic(X, y)
    probs = predict_logistic(model, X)
    auc = auc_roc(probs, y)
    threshold = calibrate_threshold_from_empty(probs, y, target_fa_rate)
    pred = probs >= threshold
    tp = int(np.sum(pred & y))
    fn = int(np.sum((~pred) & y))
    fp = int(np.sum(pred & (~y)))
    tn = int(np.sum((~pred) & (~y)))
    n_empty_windows = fp + tn
    hours = n_empty_windows * WINDOW_SECONDS / 3600.0
    fa_per_hour = fp / hours if hours > 0 else float("nan")
    recall = tp / max(tp + fn, 1) if (tp + fn) > 0 else float("nan")
    return {
        "model": model, "auc": auc, "threshold": threshold,
        "tp": tp, "fn": fn, "fp": fp, "tn": tn,
        "recall": recall, "fa_per_hour": fa_per_hour,
        "n_occupied": tp + fn, "n_empty": n_empty_windows,
    }


def detection_latency(X, y, model, starts, threshold):
    """Mean seconds from the start of an occupied run to the first window the
    model calls motion, over occupied runs that the model eventually catches."""
    probs = predict_logistic(model, X)
    pred = probs >= threshold
    y = np.asarray(y, dtype=bool)
    latencies = []
    run_start = None
    hit_logged = False
    for i in range(len(y)):
        if y[i] and run_start is None:
            run_start = i
            hit_logged = False
        if y[i] and not hit_logged and pred[i]:
            latencies.append(i - run_start)  # in windows
            hit_logged = True
        if not y[i]:
            run_start = None
            hit_logged = False
    return float(np.mean(latencies) * WINDOW_SECONDS) if latencies else float("nan")


def baseline_eval(baseline_variances, starts, labels, percentile=90):
    """Existing csi_variance + percentile baseline: flag a window as motion if the
    dominant-kind variance line nearest that window exceeds a percentile-based
    floor learned from this room's own empty-labelled windows (same spirit as the
    companion's rolling-percentile approach, computed in-sample here since this is
    an offline comparison, not a cross-validated model)."""
    # Map each report window start to the nearest baseline variance reading.
    import datetime as _dt
    bl_times = [ (_dt.datetime.fromisoformat(w), k, v) for w, k, v in baseline_variances ]
    bl_times.sort(key=lambda r: r[0])
    if not bl_times:
        return None
    # pick the most frequent kind_str as "dominant", matching firmware's own choice
    counts = defaultdict(int)
    for _, k, _ in bl_times:
        counts[k] += 1
    dominant = max(counts, key=counts.get)
    series = [(t, v) for t, k, v in bl_times if k == dominant]
    if len(series) < 5:
        return None
    series.sort(key=lambda r: r[0])
    times = [t for t, v in series]
    values = np.array([v for t, v in series])

    def nearest_value(ts):
        import bisect
        idx = bisect.bisect_left(times, ts)
        if idx == 0:
            return values[0]
        if idx >= len(times):
            return values[-1]
        before, after = times[idx - 1], times[idx]
        return values[idx - 1] if (ts - before) <= (after - ts) else values[idx]

    starts_dt = [_dt.datetime.fromisoformat(s) for s in starts]
    labels = np.asarray(labels, dtype=bool)
    readings = np.array([nearest_value(s) for s in starts_dt])
    empty_readings = readings[~labels]
    if len(empty_readings) < 5:
        return None
    floor = float(np.percentile(empty_readings, percentile))
    pred = readings > floor
    tp = int(np.sum(pred & labels))
    fn = int(np.sum((~pred) & labels))
    fp = int(np.sum(pred & (~labels)))
    tn = int(np.sum((~pred) & (~labels)))
    hours = (fp + tn) * WINDOW_SECONDS / 3600.0
    return {
        "dominant_kind": dominant, "floor": floor,
        "auc": auc_roc(readings, labels),
        "recall": tp / max(tp + fn, 1) if (tp + fn) else float("nan"),
        "fa_per_hour": fp / hours if hours > 0 else float("nan"),
        "tp": tp, "fn": fn, "fp": fp, "tn": tn,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--report-json", required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()
    with open(args.report_json) as f:
        data = json.load(f)

    lines = ["# ESPA-483 step 2: offline feature-separation report\n"]
    pooled_X, pooled_y = [], []
    for room, r in data["rooms"].items():
        lines.append(f"## {room}\n")
        lc = r["layout_check"]
        if lc:
            lines.append(f"- Bin-layout check: null-cluster mean {lc['null_mean']:.2f} vs live-bin mean {lc['live_mean']:.2f} "
                          f"(ratio {lc['ratio']:.3f}) — classic layout confirmed if ratio << 1.\n")
        lines.append(f"- {r['n_frames']} CSICAP frames, {r['n_windows_5s']} 5s windows "
                      f"({r['occupied_windows_5s']} labelled occupied), {r['n_windows_grid']} grid windows.\n")

        X5 = np.array(r["X5"]) if r["X5"] else np.zeros((0, 2))
        y5 = np.array(r["y5"], dtype=bool) if r["y5"] else np.zeros(0, dtype=bool)
        if len(X5) >= 10 and y5.sum() > 0 and (~y5).sum() > 0:
            res5 = evaluate(X5, y5)
            lat5 = detection_latency(X5, y5, res5["model"], r["starts5"], res5["threshold"])
            lines.append(f"- **ESpectre features, 5s window, fit on this room (class-balanced):** "
                          f"AUC={res5['auc']:.3f}, at 5%-FA-calibrated threshold={res5['threshold']:.2f}: "
                          f"recall={res5['recall']:.2f}, empty-room FA/hr={res5['fa_per_hour']:.2f}, "
                          f"latency={lat5:.1f}s "
                          f"(tp={res5['tp']} fn={res5['fn']} fp={res5['fp']} tn={res5['tn']}, "
                          f"n_occupied={res5['n_occupied']} n_empty={res5['n_empty']})\n")
            pooled_X.extend(X5.tolist())
            pooled_y.extend(y5.tolist())
        else:
            lines.append(f"- **ESpectre features, 5s window:** not enough labelled variety to fit "
                          f"(occupied={int(y5.sum())}, empty={int((~y5).sum())} windows) — "
                          f"{'all-empty room, FA-rate-only' if y5.sum()==0 else 'insufficient data'}.\n")

        Xg = np.array(r["Xg"]) if r["Xg"] else np.zeros((0, 2))
        yg = np.array(r["yg"], dtype=bool) if r["yg"] else np.zeros(0, dtype=bool)
        if len(Xg) >= 10 and yg.sum() > 0 and (~yg).sum() > 0:
            resg = evaluate(Xg, yg)
            lines.append(f"- **ESpectre features, {20:.0f}pps-grid admission:** AUC={resg['auc']:.3f}, "
                          f"at 5%-FA-calibrated threshold={resg['threshold']:.2f}: recall={resg['recall']:.2f}, "
                          f"empty-room FA/hr={resg['fa_per_hour']:.2f} "
                          f"(tp={resg['tp']} fn={resg['fn']} fp={resg['fp']} tn={resg['tn']})\n")

        for label, xkey, ykey in [("unicast HT/VHT", "Xuni", "yuni"), ("group/legacy", "Xgrp", "ygrp")]:
            Xk = np.array(r[xkey]) if r.get(xkey) else np.zeros((0, 2))
            yk = np.array(r[ykey], dtype=bool) if r.get(ykey) else np.zeros(0, dtype=bool)
            if len(Xk) >= 10 and yk.sum() > 0 and (~yk).sum() > 0:
                resk = evaluate(Xk, yk)
                lines.append(f"- **By kind — {label} only** ({len(yk)} windows, {int(yk.sum())} occupied): "
                              f"AUC={resk['auc']:.3f}, recall={resk['recall']:.2f}, "
                              f"empty-room FA/hr={resk['fa_per_hour']:.2f}\n")
            else:
                lines.append(f"- **By kind — {label} only:** not enough labelled variety "
                              f"({len(yk)} windows, {int(yk.sum())} occupied).\n")

        bl = baseline_eval(r["baseline_variances"], r["starts5"], r["y5"])
        if bl:
            lines.append(f"- **Existing csi_variance + percentile baseline** (dominant kind `{bl['dominant_kind']}`, "
                          f"AUC={bl['auc']:.3f}, 90th-pct empty floor={bl['floor']:.1f}): recall={bl['recall']:.2f}, "
                          f"empty-room FA/hr={bl['fa_per_hour']:.2f} "
                          f"(tp={bl['tp']} fn={bl['fn']} fp={bl['fp']} tn={bl['tn']})\n")
        else:
            lines.append("- **Existing csi_variance + percentile baseline:** not enough baseline samples to score.\n")
        lines.append("\n")

    if pooled_X:
        pooled_X = np.array(pooled_X)
        pooled_y = np.array(pooled_y, dtype=bool)
        pres = evaluate(pooled_X, pooled_y)
        lines.append(f"## Pooled (rooms with both occupied and empty labelled windows)\n")
        lines.append(f"- AUC={pres['auc']:.3f}, at 5%-FA-calibrated threshold={pres['threshold']:.2f}: "
                      f"recall={pres['recall']:.2f}, empty-room FA/hr={pres['fa_per_hour']:.2f} "
                      f"(tp={pres['tp']} fn={pres['fn']} fp={pres['fp']} tn={pres['tn']}, n={len(pooled_y)})\n")

    with open(args.out, "w") as f:
        f.writelines(lines)
    print("".join(lines))


if __name__ == "__main__":
    main()
