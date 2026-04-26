#!/usr/bin/env python3
"""Post-flash boot-loop watchdog for HIL runs.

Reproduces issue #2244 ("ESP32-C3-DevKitM-1-N4X boot loop after app entry on
ESPresense C3 builds"). After ``pio run -t upload`` flashes a board, this
script captures serial output for a short window and fails fast if it sees
any of the following bug-class signatures:

- A panic / Guru Meditation / Backtrace / abort() report.
- Two or more ROM bootloader ``rst:`` lines (i.e., the device reset more than
  once during the window — a reboot loop).
- Three or more bootloader entry headers (defense in depth against firmware
  that crashes before any panic is printed).

If any signature is hit, the script writes the captured log to stderr and
exits non-zero, so the rest of the HIL pipeline (improv-wifi pairing, full
``hil_monitor.py`` run) never executes against a known-broken build.
"""
from __future__ import annotations

import argparse
import re
import sys
import time
from dataclasses import dataclass

try:
    import serial  # type: ignore[import-not-found]
except ImportError:  # pragma: no cover -- pyserial is in the firmware-tester image
    print("pyserial is required (the firmware-tester image already has it)", file=sys.stderr)
    sys.exit(2)

RST_PATTERN = re.compile(rb"rst:0x[0-9A-Fa-f]+")
BOOT_HEADER_PATTERN = re.compile(rb"ets [A-Z][a-z]+\s+\d+\s+\d{4}")
PANIC_PATTERNS: tuple[tuple[str, re.Pattern[bytes]], ...] = (
    ("guru-meditation", re.compile(rb"Guru Meditation Error")),
    ("backtrace", re.compile(rb"Backtrace:")),
    ("abort", re.compile(rb"abort\(\) was called at PC")),
    ("panic", re.compile(rb"Core\s+\d+\s+panic'?ed", re.IGNORECASE)),
    ("assert", re.compile(rb"assert failed:")),
)


@dataclass
class Verdict:
    failed: bool = False
    reasons: list[str] = None  # type: ignore[assignment]
    rst_count: int = 0
    boot_header_count: int = 0

    def __post_init__(self) -> None:
        if self.reasons is None:
            self.reasons = []


def evaluate(buffer: bytes, *, max_rst: int, max_boot_headers: int) -> Verdict:
    verdict = Verdict()
    verdict.rst_count = len(RST_PATTERN.findall(buffer))
    verdict.boot_header_count = len(BOOT_HEADER_PATTERN.findall(buffer))

    for label, pattern in PANIC_PATTERNS:
        if pattern.search(buffer):
            verdict.reasons.append(label)
            verdict.failed = True

    if verdict.rst_count > max_rst:
        verdict.reasons.append(f"reboot-loop ({verdict.rst_count} rst: lines, > {max_rst})")
        verdict.failed = True

    if verdict.boot_header_count > max_boot_headers:
        verdict.reasons.append(
            f"reboot-loop ({verdict.boot_header_count} boot headers, > {max_boot_headers})"
        )
        verdict.failed = True

    return verdict


def capture(port: str, baud: int, window_secs: float) -> bytes:
    chunks: list[bytes] = []
    deadline = time.monotonic() + window_secs
    with serial.Serial(port, baud, timeout=0.5) as ser:
        # Toggle DTR to force a clean reset so we see the ROM bootloader output.
        # Some boards land here already-running from the post-upload reset; the
        # toggle is harmless on those.
        try:
            ser.setDTR(False)
            ser.setRTS(False)
            time.sleep(0.1)
            ser.setDTR(True)
            ser.setRTS(True)
        except Exception:  # noqa: BLE001 -- not all USB-serial bridges support this
            pass

        while time.monotonic() < deadline:
            data = ser.read(4096)
            if data:
                chunks.append(data)
    return b"".join(chunks)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n", 1)[0])
    parser.add_argument("--port", required=True, help="Serial device to monitor (e.g. /dev/esp32c3)")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--window", type=float, default=30.0, help="Capture window seconds (default: 30)")
    parser.add_argument(
        "--max-rst",
        type=int,
        default=1,
        help="Allowed rst: count in window (default: 1, the post-upload reset)",
    )
    parser.add_argument(
        "--max-boot-headers",
        type=int,
        default=2,
        help="Allowed bootloader entry headers in window (default: 2)",
    )
    parser.add_argument(
        "--log-out",
        help="Optional path to write the raw captured serial bytes for forensic upload",
    )
    args = parser.parse_args(argv)

    print(f"check_no_bootloop: capturing {args.window}s of serial from {args.port} at {args.baud} baud", flush=True)
    try:
        buffer = capture(args.port, args.baud, args.window)
    except serial.SerialException as exc:
        print(f"check_no_bootloop: serial error: {exc}", file=sys.stderr)
        return 2

    if args.log_out:
        try:
            with open(args.log_out, "wb") as fh:
                fh.write(buffer)
        except OSError as exc:
            print(f"check_no_bootloop: could not write log to {args.log_out}: {exc}", file=sys.stderr)

    verdict = evaluate(
        buffer,
        max_rst=args.max_rst,
        max_boot_headers=args.max_boot_headers,
    )

    text = buffer.decode("utf-8", errors="replace")
    print("---- captured serial ----")
    print(text)
    print("---- end captured serial ----")
    print(
        f"check_no_bootloop: rst={verdict.rst_count} boot_headers={verdict.boot_header_count}",
        flush=True,
    )

    if verdict.failed:
        print(
            f"check_no_bootloop: FAIL — {', '.join(verdict.reasons)}",
            file=sys.stderr,
        )
        print(
            "check_no_bootloop: see https://github.com/ESPresense/ESPresense/issues/2244",
            file=sys.stderr,
        )
        return 1

    print("check_no_bootloop: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
