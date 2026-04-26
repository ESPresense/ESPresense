"""Parity tests against the firmware's canonical log-distance formula.

If `lib/rssi_distance/rssi_distance.h::rssiToLogDistance` ever changes, these
fail and the harness gets re-baselined deliberately.
"""

from __future__ import annotations

import math
import unittest

from tools.accuracy_harness.firmware_model import distance_to_rssi, rssi_to_log_distance


class FirmwareModelTests(unittest.TestCase):
    def test_one_meter_at_calrssi(self):
        # By construction: rssi == calRssi -> distance == 1.0
        self.assertAlmostEqual(rssi_to_log_distance(-59.0, -59.0, 3.0), 1.0, places=6)

    def test_round_trip(self):
        # distance -> rssi -> distance should be the identity within float tol
        for d in [0.5, 1.0, 2.5, 5.0, 10.0]:
            for n in [2.0, 2.5, 3.0, 4.0]:
                rssi = distance_to_rssi(-59.0, d, n)
                self.assertAlmostEqual(
                    rssi_to_log_distance(-59.0, rssi, n),
                    d,
                    places=4,
                    msg=f"d={d}, n={n}",
                )

    def test_absorption_guard_matches_firmware(self):
        # Firmware falls back to n=2.0 when absorption <= 0
        guarded = rssi_to_log_distance(-59.0, -69.0, 0.0)
        explicit = rssi_to_log_distance(-59.0, -69.0, 2.0)
        self.assertEqual(guarded, explicit)

        guarded_neg = rssi_to_log_distance(-59.0, -69.0, -1.0)
        self.assertEqual(guarded_neg, explicit)

    def test_known_values(self):
        # Standard log-distance: 10 dB drop with n=2 -> 10x distance
        # cal=-59, rssi=-69, n=2 -> d = 10^(10/20) = sqrt(10) ~= 3.162m
        d = rssi_to_log_distance(-59.0, -69.0, 2.0)
        self.assertAlmostEqual(d, math.sqrt(10), places=4)


if __name__ == "__main__":
    unittest.main()
