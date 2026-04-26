"""Reference trilateration solver.

This is a v1 STUB intended to be replaced once Companion exposes its
`ILocate` implementations as a callable CLI / RPC (tracked in the Companion
issue filed alongside this harness). Until then, the harness uses a
pure-Python Nelder-Mead minimizing the same residual form Companion's
`NelderMeadMultilateralizer.cs` publishes:

    sum_i (||p - station_i|| - d_i)^2

We deliberately drop Companion's optional `scale` parameter and per-node
`weighting`. v1 baseline numbers are therefore a *lower bound* on
Companion's real accuracy: Companion's weighting and scale-correction can
only help, so any regression caught by this harness is a real regression.

When the Companion CLI lands, swap `solve()` over to shell out to it. The
harness's noise model and metrics stay unchanged.
"""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class StationFix:
    x: float
    y: float
    z: float
    distance: float


@dataclass(frozen=True)
class SolveResult:
    x: float
    y: float
    converged: bool
    iterations: int
    residual: float


def _residual(p: tuple[float, float], stations: Sequence[StationFix]) -> float:
    s = 0.0
    px, py = p
    for st in stations:
        # Project onto floor plane; v1 ignores z (most residential ESPresense
        # deployments place all nodes within a meter of each other vertically
        # so the 2D approximation is acceptable for the v1 baseline. Documented
        # in docs/accuracy.md blind spots).
        dx = px - st.x
        dy = py - st.y
        d = math.sqrt(dx * dx + dy * dy)
        e = d - st.distance
        s += e * e
    return s


def _nelder_mead(
    fn,
    initial: tuple[float, float],
    *,
    step: float = 0.5,
    max_iter: int = 400,
    tol: float = 1e-6,
) -> tuple[tuple[float, float], int, float]:
    """Tiny 2D Nelder-Mead, no external deps.

    Implements the standard Spendley/Nelder/Mead simplex with reflection,
    expansion, contraction, and shrink. Sufficient for the harness's
    well-behaved 2-parameter residual surface.
    """
    x0 = (initial[0], initial[1])
    simplex = [x0, (x0[0] + step, x0[1]), (x0[0], x0[1] + step)]
    fvals = [fn(p) for p in simplex]

    alpha, gamma, rho, sigma = 1.0, 2.0, 0.5, 0.5
    for it in range(max_iter):
        order = sorted(range(3), key=lambda i: fvals[i])
        simplex = [simplex[i] for i in order]
        fvals = [fvals[i] for i in order]

        spread = max(fvals) - min(fvals)
        if spread < tol:
            return simplex[0], it, fvals[0]

        # Centroid of all but worst
        cx = (simplex[0][0] + simplex[1][0]) / 2.0
        cy = (simplex[0][1] + simplex[1][1]) / 2.0

        # Reflection
        worst = simplex[2]
        xr = (cx + alpha * (cx - worst[0]), cy + alpha * (cy - worst[1]))
        fr = fn(xr)
        if fvals[0] <= fr < fvals[1]:
            simplex[2], fvals[2] = xr, fr
            continue
        if fr < fvals[0]:
            xe = (cx + gamma * (xr[0] - cx), cy + gamma * (xr[1] - cy))
            fe = fn(xe)
            if fe < fr:
                simplex[2], fvals[2] = xe, fe
            else:
                simplex[2], fvals[2] = xr, fr
            continue
        # Contraction
        xc = (cx + rho * (worst[0] - cx), cy + rho * (worst[1] - cy))
        fc = fn(xc)
        if fc < fvals[2]:
            simplex[2], fvals[2] = xc, fc
            continue
        # Shrink
        best = simplex[0]
        simplex = [
            best,
            (best[0] + sigma * (simplex[1][0] - best[0]), best[1] + sigma * (simplex[1][1] - best[1])),
            (best[0] + sigma * (simplex[2][0] - best[0]), best[1] + sigma * (simplex[2][1] - best[1])),
        ]
        fvals = [fn(p) for p in simplex]

    return simplex[0], max_iter, fvals[0]


def solve(stations: Sequence[StationFix]) -> SolveResult:
    """Locate target given a list of (station, measured_distance) fixes.

    Initial guess: weighted average of station positions, with weight
    1/distance (closer stations dominate). Mirrors Companion's
    `InitializeScenario` heuristic in spirit (closest-first ordering).
    """
    if len(stations) < 2:
        return SolveResult(math.nan, math.nan, False, 0, math.nan)

    inv_w = [1.0 / max(st.distance, 0.1) for st in stations]
    s = sum(inv_w)
    gx = sum(st.x * w for st, w in zip(stations, inv_w)) / s
    gy = sum(st.y * w for st, w in zip(stations, inv_w)) / s

    fn = lambda p: _residual(p, stations)
    pos, iters, residual = _nelder_mead(fn, (gx, gy))
    return SolveResult(pos[0], pos[1], iters < 400, iters, residual)
