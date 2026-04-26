"""End-to-end determinism + sanity tests for the runner."""

from __future__ import annotations

import unittest
from pathlib import Path

from tools.accuracy_harness.geometry import load_scenario
from tools.accuracy_harness.noise import NoiseProfile
from tools.accuracy_harness.runner import run_scenario


SCENARIOS = Path(__file__).resolve().parent.parent / "scenarios"


class RunnerTests(unittest.TestCase):
    def test_baseline_is_deterministic(self):
        scn = load_scenario(SCENARIOS / "baseline_4node_6x4m.json")
        a = run_scenario(scn, base_seed=20260426, noise=NoiseProfile())
        b = run_scenario(scn, base_seed=20260426, noise=NoiseProfile())

        self.assertEqual(len(a.tracks), len(b.tracks))
        for ta, tb in zip(a.tracks, b.tracks):
            self.assertEqual(ta.samples, tb.samples)
            self.assertAlmostEqual(ta.median_error_m, tb.median_error_m, places=10)
            self.assertAlmostEqual(ta.p95_error_m, tb.p95_error_m, places=10)
            self.assertEqual(ta.room_misid_rate, tb.room_misid_rate)

    def test_noise_free_baseline_is_near_zero_error(self):
        scn = load_scenario(SCENARIOS / "baseline_4node_6x4m.json")
        clean = NoiseProfile(
            gaussian_std_db=0.0,
            multipath_probability=0.0,
            multipath_attenuation_db=0.0,
        )
        report = run_scenario(scn, base_seed=1, noise=clean)
        for t in report.tracks:
            self.assertLess(t.median_error_m, 0.05, f"track {t.track}")
            self.assertLess(t.p95_error_m, 0.10, f"track {t.track}")
            # Noise-free misid rate is non-zero only at the room boundary
            # itself, where ~5cm solver tolerance can flip the side. Cap
            # at 5% so a real regression is still caught.
            self.assertLess(t.room_misid_rate, 0.05, f"track {t.track}")

    def test_noisy_baseline_is_finite(self):
        scn = load_scenario(SCENARIOS / "baseline_4node_6x4m.json")
        report = run_scenario(scn, base_seed=20260426, noise=NoiseProfile())
        for t in report.tracks:
            self.assertGreater(t.samples, 0)
            self.assertGreater(t.median_error_m, 0.0)
            # Sanity: with 2.5dB noise on a 6m room, median error should be
            # well under the room dimension. If this fails the solver or
            # the noise model regressed.
            self.assertLess(t.median_error_m, 2.5, f"track {t.track}")


if __name__ == "__main__":
    unittest.main()
