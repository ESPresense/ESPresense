"""Reproducible RSSI noise model.

The harness injects noise on the *RSSI* side (dBm) and then converts to
distance via the firmware formula. This is intentional: real deployments
see Gaussian-ish noise on the dB measurement, which becomes
asymmetric/heavy-tailed on the meter side. Modeling it the other way
around (Gaussian distance noise) would understate the room-misid risk.

Profiles:
  - `gaussian(std_db)` baseline RSSI jitter, dBm.
  - `multipath_burst(prob, attenuation_db)` occasional reflected/blocked
    samples that attenuate the signal by `attenuation_db` (positive
    number; the noisy RSSI is shifted *down* by that many dB).
"""

from __future__ import annotations

import random
from dataclasses import dataclass


@dataclass(frozen=True)
class NoiseProfile:
    """Tunable RSSI noise model. All RNG goes through a seeded `Random`."""

    gaussian_std_db: float = 2.5
    multipath_probability: float = 0.10
    multipath_attenuation_db: float = 6.0

    def sample(self, rng: random.Random, true_rssi: float) -> float:
        noisy = true_rssi + rng.gauss(0.0, self.gaussian_std_db)
        if rng.random() < self.multipath_probability:
            noisy -= self.multipath_attenuation_db
        return noisy


DEFAULT_PROFILE = NoiseProfile()


def make_rng(seed: int) -> random.Random:
    """Single source of truth for RNG construction in the harness.

    Per-scenario seeds are derived from a hash of (base_seed, scenario_name,
    track_name) so re-running the same baseline yields identical numbers.
    """
    return random.Random(seed)
