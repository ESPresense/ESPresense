"""Scenario runner: generate distances, solve, score.

Public entry: `run_scenario(scenario, *, base_seed, noise) -> Report`.
"""

from __future__ import annotations

import hashlib
from dataclasses import dataclass, field
from typing import Iterable

from .firmware_model import distance_to_rssi, rssi_to_log_distance
from .geometry import Scenario, Station
from .noise import NoiseProfile, make_rng
from .solver import StationFix, solve


@dataclass(frozen=True)
class TrackResult:
    track: str
    samples: int
    median_error_m: float
    p95_error_m: float
    mean_error_m: float
    max_error_m: float
    room_misid_rate: float
    single_station_outage: dict[str, dict[str, float]]


@dataclass(frozen=True)
class ScenarioReport:
    scenario: str
    base_seed: int
    noise: dict
    tracks: list[TrackResult]


def _percentile(values: list[float], p: float) -> float:
    if not values:
        return float("nan")
    s = sorted(values)
    if len(s) == 1:
        return s[0]
    k = (len(s) - 1) * p
    lo = int(k)
    hi = min(lo + 1, len(s) - 1)
    return s[lo] + (s[hi] - s[lo]) * (k - lo)


def _seed_for(base: int, scenario: str, track: str, salt: str = "") -> int:
    h = hashlib.sha256(f"{base}|{scenario}|{track}|{salt}".encode()).digest()
    return int.from_bytes(h[:8], "big", signed=False)


def _measure_one(
    truth: tuple[float, float],
    stations: Iterable[Station],
    noise: NoiseProfile,
    rng,
) -> list[StationFix]:
    fixes: list[StationFix] = []
    for st in stations:
        # 2D distance (z handled by harness simplification, see solver.py).
        import math
        dx = truth[0] - st.x
        dy = truth[1] - st.y
        true_d = math.sqrt(dx * dx + dy * dy)
        true_rssi = distance_to_rssi(st.cal_rssi, true_d, st.absorption)
        noisy_rssi = noise.sample(rng, true_rssi)
        # Round-trip through the firmware formula -- this is the whole point.
        measured_d = rssi_to_log_distance(st.cal_rssi, noisy_rssi, st.absorption)
        fixes.append(StationFix(st.x, st.y, st.z, measured_d))
    return fixes


def _score_track(
    scenario: Scenario,
    track,
    *,
    base_seed: int,
    noise: NoiseProfile,
) -> TrackResult:
    rng = make_rng(_seed_for(base_seed, scenario.name, track.name))
    points = track.expand()

    errors: list[float] = []
    misid = 0
    valid_room_samples = 0

    # Per-station outage tracking: drop station X for every sample, recompute
    # error on the remaining 3 stations.
    outage_errors: dict[str, list[float]] = {st.id: [] for st in scenario.stations}

    for x, y, _ in points:
        truth = (x, y)
        fixes = _measure_one(truth, scenario.stations, noise, rng)
        result = solve(fixes)

        import math
        if not math.isnan(result.x):
            ex = result.x - truth[0]
            ey = result.y - truth[1]
            errors.append(math.sqrt(ex * ex + ey * ey))

            true_room = scenario.room_for(truth[0], truth[1])
            est_room = scenario.room_for(result.x, result.y)
            if true_room is not None:
                valid_room_samples += 1
                if true_room != est_room:
                    misid += 1

        # Outage sweep -- drop one station at a time, re-solve.
        for i, dropped_st in enumerate(scenario.stations):
            remaining = [f for j, f in enumerate(fixes) if j != i]
            if len(remaining) < 3:
                continue
            r = solve(remaining)
            if not math.isnan(r.x):
                ex = r.x - truth[0]
                ey = r.y - truth[1]
                outage_errors[dropped_st.id].append(math.sqrt(ex * ex + ey * ey))

    median = _percentile(errors, 0.5)
    p95 = _percentile(errors, 0.95)
    mean = sum(errors) / len(errors) if errors else float("nan")
    mx = max(errors) if errors else float("nan")
    misid_rate = (misid / valid_room_samples) if valid_room_samples else 0.0

    outage_summary: dict[str, dict[str, float]] = {}
    for sid, errs in outage_errors.items():
        if not errs:
            outage_summary[sid] = {"samples": 0, "median_error_m": float("nan"), "p95_error_m": float("nan")}
        else:
            outage_summary[sid] = {
                "samples": len(errs),
                "median_error_m": _percentile(errs, 0.5),
                "p95_error_m": _percentile(errs, 0.95),
            }

    return TrackResult(
        track=track.name,
        samples=len(errors),
        median_error_m=median,
        p95_error_m=p95,
        mean_error_m=mean,
        max_error_m=mx,
        room_misid_rate=misid_rate,
        single_station_outage=outage_summary,
    )


def run_scenario(
    scenario: Scenario,
    *,
    base_seed: int = 1,
    noise: NoiseProfile | None = None,
) -> ScenarioReport:
    profile = noise or NoiseProfile()
    track_results = [
        _score_track(scenario, t, base_seed=base_seed, noise=profile)
        for t in scenario.tracks
    ]
    return ScenarioReport(
        scenario=scenario.name,
        base_seed=base_seed,
        noise={
            "gaussian_std_db": profile.gaussian_std_db,
            "multipath_probability": profile.multipath_probability,
            "multipath_attenuation_db": profile.multipath_attenuation_db,
        },
        tracks=track_results,
    )
