"""Solver sanity tests.

Verifies the reference Nelder-Mead converges to within ~5cm on noise-free
inputs from a 4-station perimeter setup. This is a regression guard on the
solver itself; deliberately easier than the harness baseline so the tests
fail loud if the optimizer breaks.
"""

from __future__ import annotations

import math
import unittest

from tools.accuracy_harness.solver import StationFix, solve


class SolverTests(unittest.TestCase):
    STATIONS_4 = [
        (0.0, 0.0),
        (6.0, 0.0),
        (6.0, 4.0),
        (0.0, 4.0),
    ]

    def _fixes_for(self, target):
        out = []
        for sx, sy in self.STATIONS_4:
            d = math.sqrt((target[0] - sx) ** 2 + (target[1] - sy) ** 2)
            out.append(StationFix(sx, sy, 0.0, d))
        return out

    def test_noise_free_center(self):
        target = (3.0, 2.0)
        r = solve(self._fixes_for(target))
        self.assertTrue(r.converged)
        self.assertLess(math.hypot(r.x - target[0], r.y - target[1]), 0.05)

    def test_noise_free_offcenter(self):
        for target in [(1.0, 1.0), (5.0, 3.0), (4.5, 0.5)]:
            r = solve(self._fixes_for(target))
            self.assertTrue(r.converged, target)
            self.assertLess(
                math.hypot(r.x - target[0], r.y - target[1]),
                0.05,
                msg=f"target={target}",
            )

    def test_under_two_stations_returns_nan(self):
        r = solve([StationFix(0.0, 0.0, 0.0, 1.0)])
        self.assertTrue(math.isnan(r.x))


if __name__ == "__main__":
    unittest.main()
