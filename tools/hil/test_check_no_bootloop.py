"""Offline tests for the boot-loop signature evaluator.

The ``capture`` step is excluded from these tests because it requires a real
serial device; the heuristic in ``evaluate`` is what determines pass/fail and
is the load-bearing part to lock down.
"""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from check_no_bootloop import evaluate  # noqa: E402


HEALTHY_BOOT = b"""
ets Jul 29 2019 12:21:46
rst:0x1 (POWERON_RESET)
configsip: 0
SPIWP:0xee
clk_drv:0x00,q_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:0x6f4
entry 0x40080638
ESPresense booted, IP=10.0.0.42
"""

C3_BOOTLOOP = b"""
ets Jul 29 2019 12:21:46
rst:0x1 (POWERON_RESET)
entry 0x403823ac
SHA-256 comparison failed.
rst:0x7 (TG0WDT_SYS_RST)
ets Jul 29 2019 12:21:46
rst:0x10 (RTCWDT_RTC_RST)
ets Jul 29 2019 12:21:46
"""

PANIC = b"""
ets Jul 29 2019 12:21:46
rst:0x1 (POWERON_RESET)
abort() was called at PC 0x40080638 on core 0
Backtrace: 0x40080638:0x3ffb0080 0x4008063b:0x3ffb00a0
"""

GURU = b"""
ets Jul 29 2019 12:21:46
rst:0x1 (POWERON_RESET)
Guru Meditation Error: Core 0 panic'ed (Unhandled debug exception).
"""


class EvaluateTests(unittest.TestCase):
    def test_healthy_boot(self) -> None:
        v = evaluate(HEALTHY_BOOT, max_rst=1, max_boot_headers=2)
        self.assertFalse(v.failed)
        self.assertEqual(v.rst_count, 1)
        self.assertEqual(v.boot_header_count, 1)

    def test_c3_bootloop(self) -> None:
        v = evaluate(C3_BOOTLOOP, max_rst=1, max_boot_headers=2)
        self.assertTrue(v.failed)
        self.assertGreaterEqual(v.rst_count, 3)
        self.assertTrue(any("reboot-loop" in r for r in v.reasons))

    def test_panic_with_backtrace(self) -> None:
        v = evaluate(PANIC, max_rst=1, max_boot_headers=2)
        self.assertTrue(v.failed)
        self.assertIn("backtrace", v.reasons)
        self.assertIn("abort", v.reasons)

    def test_guru_meditation(self) -> None:
        v = evaluate(GURU, max_rst=1, max_boot_headers=2)
        self.assertTrue(v.failed)
        self.assertIn("guru-meditation", v.reasons)
        self.assertIn("panic", v.reasons)

    def test_threshold_overrides(self) -> None:
        # If a board is allowed to reboot once on first boot (rare), a more
        # permissive threshold lets it pass.
        v = evaluate(C3_BOOTLOOP, max_rst=10, max_boot_headers=10)
        self.assertFalse(v.failed)


if __name__ == "__main__":
    unittest.main()
