#!/usr/bin/env python3
"""
ESPA-483 step 2: offline evaluation of ESpectre's lightweight-detector features
against our passive, sparse CSI captures, with Home Assistant occupancy history
as ground truth.

Algorithm constants (Hampel window/threshold, aggregation width, fitted-logistic
functional form) are taken from github.com/francescopace/espectre (GPL-3.0),
src/cpp/core/{filter_config,csi_features,lightweight_detector}.h. We fit our own
two weights on our own data rather than reusing ESpectre's trained coefficients,
since those were fit on their active, 100pps, dense stream.

Usage:
    python3 espa483_analyze.py --capture-dir <dir> --ha-history <dir> --report <out.md>
"""
import argparse
import gzip
import glob
import json
import math
import os
import re
import sys
from collections import defaultdict
from datetime import datetime, timezone, timedelta

import numpy as np

# =============================================================================
# ESpectre constants (see src/cpp/core/{filter_config,csi_features}.h)
# =============================================================================
HAMPEL_WINDOW = 7
HAMPEL_THRESHOLD = 5.0  # multiples of MAD
MAD_SCALE = 1.4826
AGGR_WIDTH = 5  # TURB_IQR_AGGREGATION_WIDTH
GUARD_LOW_CENTERED = 4
GUARD_HIGH_CENTERED = 60
DC_CENTERED = 32
DEFAULT_SUBCARRIERS_CENTERED = [4, 8, 13, 18, 23, 28, 36, 41, 46, 51, 56, 60]

WINDOW_SECONDS = 5.0


def centered_to_raw_classic(c):
    """Classic-MAC (esp32/esp32c3/esp32s3) native bin order: bin = subcarrier mod 64,
    DC at bin 0. Centered bin c represents physical subcarrier c-32; converting that
    subcarrier to classic raw index is (c-32) mod 64 == (c+32) mod 64."""
    return (c + 32) % 64


# =============================================================================
# Parsing
# =============================================================================
CSICAP_RE = re.compile(r"^CSICAP (\d+) (\d+) (\d+) (\d+) (.+)$")
BASELINE_RE = re.compile(r"^CSI: window(.*?) dropped=(\d+)(.*)$")
BASELINE_KIND_RE = re.compile(r"(uni|grp)/(\d+)/(\d+):(\d+)=([\d.]+)")
SEGMENT_TS_RE = re.compile(r"_(\d{8}T\d{6}Z)\.log\.gz$")


def parse_segment_start(path):
    m = SEGMENT_TS_RE.search(path)
    if not m:
        return None
    return datetime.strptime(m.group(1), "%Y%m%dT%H%M%SZ").replace(tzinfo=timezone.utc)


def _parse_amps(amp_str, count):
    """Fast path (C-level) first; falls back only for the rare truncated line at
    a segment boundary (the 600s rotation cutoff doesn't land on a line boundary)."""
    try:
        amps = np.fromstring(amp_str, sep=" ", dtype=np.float64)
    except (ValueError, AttributeError):
        return None
    if len(amps) != count:
        return None
    return amps


def load_room(capture_dir, room, target_raw, aggr_groups):
    """Streams every segment once: per CSICAP line, computes (turb, turb_aggr)
    immediately and discards the 64-float amplitude array rather than keeping it
    (millions of frames x 64 floats would be gigabytes). `target_raw`/`aggr_groups`
    come from prescan_dead_mask()+empirical_targets(), adapted to this room's
    measured dead bins.

    Returns (per_frame, baseline):
    per_frame: list of (wall, kind, turb, turb_aggr)
    baseline: list of dicts {wall, kind_str, variance, frames}
    """
    paths = sorted(glob.glob(os.path.join(capture_dir, f"{room}_*.log.gz")))
    per_frame = []
    baseline = []
    anchor_wall = None
    anchor_ms = None
    last_ms = None
    last_wall = None
    for path in paths:
        seg_start = parse_segment_start(path)
        with gzip.open(path, "rt", errors="replace") as f:
            for line in f:
                if line.startswith("CSICAP "):
                    parts = line.split(" ", 5)
                    if len(parts) != 6:
                        continue
                    try:
                        ts_ms = int(parts[1])
                        kind = int(parts[2])
                        count = int(parts[4])
                    except ValueError:
                        continue
                    amps = _parse_amps(parts[5], count)
                    if amps is None or count != 64:
                        continue
                    if anchor_wall is None:
                        anchor_wall = seg_start
                        anchor_ms = ts_ms
                    if last_ms is not None and ts_ms < last_ms - 1000:
                        # millis() went backwards by more than jitter: the node rebooted.
                        anchor_wall = seg_start
                        anchor_ms = ts_ms
                    last_ms = ts_ms
                    wall = anchor_wall + timedelta(seconds=(ts_ms - anchor_ms) / 1000.0)
                    last_wall = wall
                    raw12 = amps[target_raw]
                    turb = cv_turbulence(raw12)
                    aggr12 = np.array([amps[idxs].mean() for idxs in aggr_groups])
                    turb_aggr = cv_turbulence(aggr12)
                    per_frame.append((wall, kind, turb, turb_aggr))
                    continue
                m = BASELINE_RE.match(line.rstrip("\n"))
                if m and anchor_wall is not None:
                    wall = last_wall if last_wall is not None else seg_start
                    for km in BASELINE_KIND_RE.finditer(m.group(1)):
                        group, sigmode, rate_or_mcs, nframes, variance = km.groups()
                        baseline.append({
                            "wall": wall,
                            "kind_str": f"{group}/{sigmode}/{rate_or_mcs}",
                            "frames": int(nframes),
                            "variance": float(variance),
                        })
    per_frame.sort(key=lambda r: r[0])
    baseline.sort(key=lambda r: r["wall"])
    return per_frame, baseline


# =============================================================================
# Bin-layout check (empirical, not assumed) -- and empirical target selection.
#
# A first real run found that ESpectre's own HT20_CLASSIC_ONLY_NULL_BINS (7 bins
# around centered DC, i.e. raw indices {0,29..35} via the (c+32)%64 mapping) does
# not match our actual hardware: the measured null band is 9 bins wide (raw
# 28..36 inclusive, confirmed to read exactly 0.0 across tens of thousands of
# frames of the dominant kind), and raw index 0 is not DC but first-word-invalid
# garbage (consistently large and NOT physically plausible as a channel reading;
# raw index 1 is the real, small-but-nonzero DC). Two of ESpectre's twelve
# default tones (centered 28 and 36, their closest-to-DC choice) map straight
# into that wider dead zone. Rather than hardcode a corrected offset table,
# determine the live/dead split from the data itself and nudge any colliding
# target outward (away from center) to the nearest live bin, same spirit as the
# existing CsiWindow.h convention of never trusting the first subcarrier pair.
# =============================================================================
DEAD_RAW_FIXED = {0, 1}  # first_word_invalid garbage + its DC neighbor; never trusted.
DEAD_AMPLITUDE_THRESHOLD = 1.0  # a true null bin reads exactly 0.0; this is a generous margin.


def empirical_dead_mask(amp_sample):
    """bool[64]: True where this room's hardware/driver reliably reads ~0 (or is
    in DEAD_RAW_FIXED), from a sample of real frames of any kind."""
    mask = np.zeros(64, dtype=bool)
    for i in DEAD_RAW_FIXED:
        mask[i] = True
    if amp_sample:
        arr = np.vstack(amp_sample)
        if arr.shape[0] >= 20:
            mean_energy = arr.mean(axis=0)
            mask |= mean_energy < DEAD_AMPLITUDE_THRESHOLD
    return mask


def check_classic_layout_from_sample(amp_sample, dead_mask):
    """Report the measured null-vs-live contrast for the report/sanity check."""
    if not amp_sample:
        return None
    arr = np.vstack(amp_sample)
    if arr.shape[0] < 20:
        return None
    mean_energy = arr.mean(axis=0)
    dead_idx = np.where(dead_mask)[0]
    live_idx = np.where(~dead_mask)[0]
    null_mean = mean_energy[dead_idx].mean() if len(dead_idx) else float("nan")
    live_mean = mean_energy[live_idx].mean() if len(live_idx) else float("nan")
    return {"null_mean": float(null_mean), "live_mean": float(live_mean),
            "ratio": float(null_mean / max(live_mean, 1e-9)),
            "n_sampled": int(arr.shape[0]), "dead_raw_indices": dead_idx.tolist()}


def nudge_to_live_center(target_centered, dead_mask):
    """If (target+32)%64 is dead, find the nearest live, in-bounds centered bin.
    Prefers moving away from DC first (matches the intent of picking tones away
    from the null region), but a target already sitting at the GUARD_LOW/HIGH
    edge (e.g. centered 60, ESpectre's own outermost choice) has nowhere further
    out to go, so this also tries inward and picks whichever direction reaches a
    live bin in fewer steps."""
    raw0 = centered_to_raw_classic(target_centered)
    if not dead_mask[raw0]:
        return target_centered
    away = 1 if target_centered >= DC_CENTERED else -1
    for distance in range(1, 20):
        for direction in (away, -away):
            c = target_centered + direction * distance
            if not (GUARD_LOW_CENTERED <= c <= GUARD_HIGH_CENTERED):
                continue
            if not dead_mask[centered_to_raw_classic(c)]:
                return c
    return target_centered  # give up, caller will just get a dead/zero reading


def empirical_targets(dead_mask):
    """Adjusted (TARGET_RAW, AGGR_RAW_GROUPS) for this room's measured dead bins."""
    adjusted_centered = [nudge_to_live_center(c, dead_mask) for c in DEFAULT_SUBCARRIERS_CENTERED]
    target_raw = [centered_to_raw_classic(c) for c in adjusted_centered]
    aggr_groups = []
    for c in adjusted_centered:
        half = (AGGR_WIDTH - 1) // 2
        low, high = c - half, c + (AGGR_WIDTH - 1) - half
        if low < GUARD_LOW_CENTERED:
            low, high = GUARD_LOW_CENTERED, GUARD_LOW_CENTERED + AGGR_WIDTH - 1
        if high > GUARD_HIGH_CENTERED:
            low, high = GUARD_HIGH_CENTERED - AGGR_WIDTH + 1, GUARD_HIGH_CENTERED
        raws = [centered_to_raw_classic(cc) for cc in range(low, high + 1) if cc != DC_CENTERED]
        raws = [r for r in raws if not dead_mask[r]] or [centered_to_raw_classic(c)]
        aggr_groups.append(raws)
    return target_raw, aggr_groups, adjusted_centered


def prescan_dead_mask(capture_dir, room, max_lines=60000):
    """Read a bounded prefix of this room's capture (fast) to build the dead-bin
    mask before the full streaming pass, so target selection adapts per room
    instead of assuming all chips share one fixed null geometry."""
    paths = sorted(glob.glob(os.path.join(capture_dir, f"{room}_*.log.gz")))
    sample = []
    n = 0
    for path in paths:
        with gzip.open(path, "rt", errors="replace") as f:
            for line in f:
                if not line.startswith("CSICAP "):
                    continue
                parts = line.split(" ", 5)
                if len(parts) != 6:
                    continue
                try:
                    count = int(parts[4])
                except ValueError:
                    continue
                amps = _parse_amps(parts[5], count)
                if amps is None or count != 64:
                    continue
                sample.append(amps)
                n += 1
                if n >= max_lines:
                    break
        if n >= max_lines:
            break
    return empirical_dead_mask(sample), sample


def cv_turbulence(amps_12):
    mean = amps_12.mean()
    if mean <= 0:
        return 0.0
    return float(amps_12.std() / mean)


def _median(sorted_or_unsorted):
    """Pure-Python median of a short list. Called a few hundred thousand times
    per room on windows of <= HAMPEL_WINDOW=7 elements; numpy's per-call
    dispatch overhead dominates at that size, so this avoids numpy entirely in
    the hot loop (this was the actual bottleneck in a first full run)."""
    s = sorted(sorted_or_unsorted)
    n = len(s)
    mid = n // 2
    return s[mid] if n % 2 else (s[mid - 1] + s[mid]) / 2.0


def hampel_filter_series(values):
    """Stateful, causal Hampel filter matching ESpectre's hampel_filter_turbulence:
    a sliding window of up to HAMPEL_WINDOW most recent (causal) samples, median +
    MAD, replace the current sample with the median if it's an outlier."""
    out = [0.0] * len(values)
    buf = []
    for i, v in enumerate(values):
        v = float(v)
        window = buf + [v]
        med = _median(window)
        mad = _median([abs(x - med) for x in window])
        threshold = HAMPEL_THRESHOLD * MAD_SCALE * mad
        out[i] = med if (threshold > 0 and abs(v - med) > threshold) else v
        buf.append(v)
        if len(buf) > HAMPEL_WINDOW:
            buf.pop(0)
    return np.array(out)


def lag1_autocorr(x):
    n = len(x)
    if n < 3:
        return 0.0
    mean = x.mean()
    var = ((x - mean) ** 2).mean()
    if var < 1e-10:
        return 0.0
    cov = ((x[:-1] - mean) * (x[1:] - mean)).mean()
    return float(cov / var)


def iqr_over_mean(x):
    if len(x) < 2:
        return 0.0
    mean = x.mean()
    q75, q25 = np.percentile(x, [75, 25])
    return float((q75 - q25) / max(abs(mean), 1e-6))


def is_unicast_ht(kind):
    """Matches CsiMotion.cpp's kindOf(): bit15 = group-addressed, bits8-9 = sig_mode
    (0 = legacy rate). 'uni/1/*' and 'uni/3/*' (HT/VHT unicast) vs everything else
    (group-addressed of any kind, or legacy-rate unicast) -- the same split #2601
    found mixing-sensitive."""
    is_group = bool(kind & 0x8000)
    sig_mode = (kind >> 8) & 3
    return (not is_group) and sig_mode != 0


def build_windows(per_frame, target_pps_grid=None, t0=None, kind_filter=None):
    """Bucket frames into WINDOW_SECONDS-wide, non-overlapping wall-clock windows.
    Returns list of dicts: window_start, window_end, n_frames, autocorr, iqr_over_mean_aggr.
    `t0` lets kind-filtered subsets share the same window boundaries as the
    unfiltered run, so a by-kind breakdown is comparing the same clock, not
    independently-anchored windows."""
    if kind_filter is not None:
        per_frame = [f for f in per_frame if kind_filter(f[1])]
    if not per_frame:
        return []
    t0 = t0 if t0 is not None else per_frame[0][0]
    buckets = defaultdict(list)
    for wall, kind, turb, turb_aggr in per_frame:
        idx = int((wall - t0).total_seconds() // WINDOW_SECONDS)
        buckets[idx].append((wall, turb, turb_aggr))
    windows = []
    for idx in sorted(buckets):
        rows = buckets[idx]
        if target_pps_grid:
            # Admit at most one sample per 1/target_pps_grid slot within the window,
            # closest to slot center (same intent as ESpectre's fixed-grid sampler,
            # simplified for an offline, already-ordered dataset).
            slot = 1.0 / target_pps_grid
            win_start = t0 + timedelta(seconds=idx * WINDOW_SECONDS)
            slots = {}
            for wall, turb, turb_aggr in rows:
                offset = (wall - win_start).total_seconds()
                slot_idx = int(offset // slot)
                center = slot_idx * slot + slot / 2
                dist = abs(offset - center)
                if slot_idx not in slots or dist < slots[slot_idx][0]:
                    slots[slot_idx] = (dist, wall, turb, turb_aggr)
            rows = [(w, t, ta) for _, w, t, ta in slots.values()]
            rows.sort(key=lambda r: r[0])
        if len(rows) < 3:
            continue
        turbs = hampel_filter_series(np.array([r[1] for r in rows]))
        aggrs = hampel_filter_series(np.array([r[2] for r in rows]))
        windows.append({
            "start": t0 + timedelta(seconds=idx * WINDOW_SECONDS),
            "end": t0 + timedelta(seconds=(idx + 1) * WINDOW_SECONDS),
            "n_frames": len(rows),
            "autocorr": lag1_autocorr(turbs),
            "iqr_over_mean_aggr": iqr_over_mean(aggrs),
        })
    return windows


# =============================================================================
# HA occupancy labelling
# =============================================================================
def load_ha_states(path):
    with open(path) as f:
        doc = json.load(f)
    states = []
    for entity in doc["data"]["entities"]:
        for s in entity["states"]:
            ts = datetime.fromisoformat(s["last_changed"].replace("Z", "+00:00"))
            if ts.tzinfo is None:
                ts = ts.replace(tzinfo=timezone.utc)
            ts = ts.astimezone(timezone.utc)
            states.append((ts, s["state"]))
    states.sort(key=lambda r: r[0])
    return states


def occupied_during(states, start, end):
    """True if the sensor reported 'on' at any point overlapping [start, end)."""
    # State at window start: last state at or before `start`.
    current = "off"
    for ts, state in states:
        if ts <= start:
            current = state
        else:
            break
    if current == "on":
        return True
    for ts, state in states:
        if start < ts < end and state == "on":
            return True
    return False


# =============================================================================
# Metrics
# =============================================================================
def sigmoid(x):
    return 1.0 / (1.0 + math.exp(-max(min(x, 20), -20)))


def fit_logistic(X, y, iters=4000, lr=0.5, class_weight="balanced"):
    """Plain gradient-descent logistic regression on standardized 2-feature X.
    Avoids an sklearn dependency for a tools/ script run ad hoc.

    class_weight="balanced" weights each sample by 1/(its class's frequency),
    same intent as sklearn's class_weight. Without this, an unweighted fit on
    our heavily imbalanced rooms (e.g. laundry is 5.7% occupied, family is 68%
    occupied) converges to a near-constant majority-class predictor -- high
    apparent "accuracy" that says nothing about whether the two features
    actually separate occupied from empty."""
    X = np.asarray(X, dtype=float)
    y = np.asarray(y, dtype=float)
    mu = X.mean(axis=0)
    sigma = X.std(axis=0)
    sigma[sigma < 1e-9] = 1.0
    Xz = (X - mu) / sigma
    n = len(y)
    if class_weight == "balanced":
        n_pos = max(y.sum(), 1)
        n_neg = max(n - y.sum(), 1)
        sample_w = np.where(y > 0, n / (2 * n_pos), n / (2 * n_neg))
    else:
        sample_w = np.ones(n)
    sample_w = sample_w / sample_w.sum() * n  # keep gradient scale comparable to unweighted
    w = np.zeros(2)
    b = 0.0
    for _ in range(iters):
        z = Xz @ w + b
        p = 1.0 / (1.0 + np.exp(-np.clip(z, -20, 20)))
        grad_w = Xz.T @ (sample_w * (p - y)) / n
        grad_b = (sample_w * (p - y)).mean()
        w -= lr * grad_w
        b -= lr * grad_b
    return {"mu": mu.tolist(), "sigma": sigma.tolist(), "w": w.tolist(), "b": float(b)}


def auc_roc(probs, labels):
    """Rank-based, threshold-independent separation metric (probability a random
    occupied window scores higher than a random empty one). 0.5 = no
    separation, 1.0 = perfect. Reported first because it answers the issue's
    actual question -- do these features separate at all -- before any
    particular operating threshold, which a skewed class balance can make
    look arbitrarily good or bad on its own."""
    labels = np.asarray(labels, dtype=bool)
    pos = probs[labels]
    neg = probs[~labels]
    if len(pos) == 0 or len(neg) == 0:
        return float("nan")
    # Mann-Whitney U / rank-sum form, O((n_pos+n_neg) log n) via argsort.
    all_scores = np.concatenate([pos, neg])
    order = np.argsort(all_scores)
    ranks = np.empty_like(order, dtype=float)
    ranks[order] = np.arange(1, len(all_scores) + 1)
    # Average ties.
    sorted_scores = all_scores[order]
    sorted_ranks = ranks[order]
    i = 0
    while i < len(sorted_scores):
        j = i
        while j + 1 < len(sorted_scores) and sorted_scores[j + 1] == sorted_scores[i]:
            j += 1
        if j > i:
            avg = sorted_ranks[i:j + 1].mean()
            sorted_ranks[i:j + 1] = avg
        i = j + 1
    ranks[order] = sorted_ranks
    rank_sum_pos = ranks[: len(pos)].sum()
    n_pos, n_neg = len(pos), len(neg)
    u = rank_sum_pos - n_pos * (n_pos + 1) / 2
    return float(u / (n_pos * n_neg))


def calibrate_threshold_from_empty(probs, labels, target_fa_rate=0.05):
    """Pick the threshold the way ESpectre's own design actually calibrates --
    from quiet/empty data alone ('~10s of quiet room at boot'), not by
    optimizing a mixed-label metric like F1. That matters here specifically
    because a majority-occupied room (e.g. family, 68% of windows) makes
    F1-optimization degenerate to an always-positive threshold: F1 for "always
    guess occupied" already beats most real operating points when occupied is
    the majority class, which produces a technically-maximal-F1 but useless
    (near-0 threshold, near-100% false-alarm) operating point. Calibrating
    from the empty windows' own score distribution instead targets a fixed,
    meaningful false-alarm rate regardless of how occupied/empty the capture
    happened to be."""
    labels = np.asarray(labels, dtype=bool)
    empty_probs = probs[~labels]
    if len(empty_probs) < 10:
        return float("nan")
    return float(np.percentile(empty_probs, 100 * (1 - target_fa_rate)))


def best_f1_threshold(probs, labels, n_steps=199):
    """Scan thresholds, return the one maximizing F1 -- a principled single
    operating point to report recall/FA-rate at, instead of an arbitrary 0.5
    that a balanced-weighted fit doesn't particularly privilege either."""
    labels = np.asarray(labels, dtype=bool)
    best_t, best_f1 = 0.5, -1.0
    for t in np.linspace(0.01, 0.99, n_steps):
        pred = probs >= t
        tp = np.sum(pred & labels)
        fp = np.sum(pred & ~labels)
        fn = np.sum(~pred & labels)
        if tp == 0:
            continue
        precision = tp / (tp + fp)
        recall = tp / (tp + fn)
        f1 = 2 * precision * recall / (precision + recall) if (precision + recall) > 0 else 0
        if f1 > best_f1:
            best_f1, best_t = f1, t
    return float(best_t)


def predict_logistic(model, X):
    X = np.asarray(X, dtype=float)
    mu = np.array(model["mu"])
    sigma = np.array(model["sigma"])
    w = np.array(model["w"])
    Xz = (X - mu) / sigma
    z = Xz @ w + model["b"]
    return 1.0 / (1.0 + np.exp(-np.clip(z, -20, 20)))


def score_at_threshold(probs, labels, threshold):
    pred = probs >= threshold
    labels = np.asarray(labels, dtype=bool)
    tp = int(np.sum(pred & labels))
    fn = int(np.sum((~pred) & labels))
    fp = int(np.sum(pred & (~labels)))
    tn = int(np.sum((~pred) & (~labels)))
    recall = tp / max(tp + fn, 1)
    return {"tp": tp, "fn": fn, "fp": fp, "tn": tn, "recall": recall}


def process_room(capture_dir, ha_dir, room, grid_pps):
    """Everything for one room: prescan, full streaming pass, windowing, HA
    labelling. Run in its own process (see main()) since gzip decompression +
    parsing millions of lines is CPU-bound and the GIL would serialize it
    otherwise -- this is the unit of parallelism across the 3 labelled rooms."""
    ha_path = os.path.join(ha_dir, f"{room}_ha.json")
    if not os.path.exists(ha_path):
        print(f"[{room}] no HA history file, skipping", file=sys.stderr)
        return room, None
    print(f"[{room}] prescanning for dead bins...", file=sys.stderr)
    dead_mask, prescan_sample = prescan_dead_mask(capture_dir, room)
    target_raw, aggr_groups, adjusted_centered = empirical_targets(dead_mask)
    layout_uni = check_classic_layout_from_sample(prescan_sample, dead_mask)
    print(f"[{room}] dead raw bins: {sorted(int(i) for i in np.where(dead_mask)[0])}", file=sys.stderr)
    print(f"[{room}] adjusted centered targets: {adjusted_centered} (default {DEFAULT_SUBCARRIERS_CENTERED})",
          file=sys.stderr)
    print(f"[{room}] loading capture...", file=sys.stderr)
    import time as _time
    _t_start = _time.time()
    per_frame, baseline = load_room(capture_dir, room, target_raw, aggr_groups)
    print(f"[{room}] {len(per_frame)} CSICAP frames, {len(baseline)} baseline records, "
          f"{_time.time() - _t_start:.0f}s", file=sys.stderr)
    if not per_frame:
        return room, None
    wall_t0 = per_frame[0][0]
    windows_5s = build_windows(per_frame, t0=wall_t0)
    windows_grid = build_windows(per_frame, target_pps_grid=grid_pps, t0=wall_t0)
    # By frame kind (#2601 found mixing kinds reads noisy): same window clock,
    # just restricted to one kind-group's frames.
    windows_uni_ht = build_windows(per_frame, t0=wall_t0, kind_filter=is_unicast_ht)
    windows_grp_legacy = build_windows(per_frame, t0=wall_t0, kind_filter=lambda k: not is_unicast_ht(k))
    states = load_ha_states(ha_path)

    def label_windows(windows):
        X, y, starts = [], [], []
        for w in windows:
            X.append([w["autocorr"], w["iqr_over_mean_aggr"]])
            y.append(occupied_during(states, w["start"], w["end"]))
            starts.append(w["start"])
        return np.array(X), np.array(y, dtype=bool), starts

    X5, y5, starts5 = label_windows(windows_5s)
    Xg, yg, startsg = label_windows(windows_grid)
    Xuni, yuni, startsuni = label_windows(windows_uni_ht)
    Xgrp, ygrp, startsgrp = label_windows(windows_grp_legacy)

    return room, {
        "layout_check": layout_uni,
        "dead_raw_bins": sorted(int(i) for i in np.where(dead_mask)[0]),
        "adjusted_centered_targets": adjusted_centered,
        "n_frames": len(per_frame),
        "n_windows_5s": len(windows_5s),
        "n_windows_grid": len(windows_grid),
        "occupied_windows_5s": int(y5.sum()) if len(y5) else 0,
        "X5": X5.tolist(), "y5": y5.tolist(), "starts5": [s.isoformat() for s in starts5],
        "Xg": Xg.tolist(), "yg": yg.tolist(),
        "Xuni": Xuni.tolist(), "yuni": yuni.tolist(), "startsuni": [s.isoformat() for s in startsuni],
        "Xgrp": Xgrp.tolist(), "ygrp": ygrp.tolist(), "startsgrp": [s.isoformat() for s in startsgrp],
        "baseline_variances": [(b["wall"].isoformat(), b["kind_str"], b["variance"]) for b in baseline],
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--capture-dir", required=True)
    ap.add_argument("--ha-dir", required=True, help="dir with <room>_ha.json files")
    ap.add_argument("--rooms", nargs="+", default=["office", "laundry", "family", "t_embed"])
    ap.add_argument("--report", required=True)
    ap.add_argument("--grid-pps", type=float, default=20.0)
    args = ap.parse_args()

    import concurrent.futures as cf
    report = {"rooms": {}}
    with cf.ProcessPoolExecutor(max_workers=len(args.rooms)) as pool:
        futures = [pool.submit(process_room, args.capture_dir, args.ha_dir, room, args.grid_pps)
                   for room in args.rooms]
        for fut in cf.as_completed(futures):
            room, result = fut.result()
            if result is not None:
                report["rooms"][room] = result

    with open(args.report, "w") as f:
        json.dump(report, f, default=str)
    print(f"wrote {args.report}", file=sys.stderr)


if __name__ == "__main__":
    main()
