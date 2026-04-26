# RSSI / distance test fixtures

This directory documents the fixture corpus consumed by
`test/test_native_distance/` (run via `pio test -e native`). Every fixture
is a frozen `(inputs) -> expected output ± tolerance` row that PR CI
asserts on every push, so a regression in either the log-distance formula
or the `AdaptivePercentileRSSI` filter math fails the build before the
firmware artifact is uploaded.

The native test binary is the only consumer right now. Live fixtures are
embedded as C++ static arrays in
`test/test_native_distance/fixtures.h` for build hermeticity. CSV copies
in this directory exist so contributors can author / review without
reading C++.

## Fixture types

### Formula fixtures (`formula_fixtures.csv`)

Pin the output of `rssiToLogDistance(calRssi, rssi, absorption)` from
`lib/rssi_distance/rssi_distance.h`, the canonical implementation called
from `src/BleFingerprint.cpp`.

Columns:

| column            | unit  | meaning                                                           |
|-------------------|-------|-------------------------------------------------------------------|
| name              |       | fixture id, used in failure messages                              |
| board             |       | tier-1 board variant the calibration is typical for               |
| calRssi           | dBm   | RSSI at 1 m reference                                             |
| rssi              | dBm   | observed RSSI                                                     |
| absorption        |       | path-loss exponent (n); 2.0 = free-space, ~3.0 indoor, 3.5 walls  |
| expectedDistance  | m     | hand-derived from the formula                                     |
| toleranceMeters   | m     | absolute tolerance permitted on the host float32 path             |
| note              |       | provenance / why the row exists                                   |

### Filter fixtures (`filter_fixtures.csv`)

Pin the output of `AdaptivePercentileRSSI::getMedianIQR()` for synthetic
RSSI streams (Tukey-fence-trimmed mean, k=1.5). Sample arrays live in
`fixtures.h` because Unity tests are easier to debug with named arrays
than CSV indexing.

## Adding a fixture from a captured RSSI log

1. **Capture** the log on real hardware. Easiest path: enable verbose
   logging on a tier-1 node, hold a known beacon at a measured distance,
   record the per-advertisement RSSI for ~30 s (so the 15 s rolling
   window fills).
2. **Trim** the capture to a clean window (no movement, no
   interference). Aim for 20–40 readings.
3. **Derive expected** values:
   - For a steady-distance capture, expected `getMedianIQR()` is the
     mean of the inner quartile range of the trimmed window.
   - For the corresponding distance fixture, plug the expected smoothed
     RSSI into `rssiToLogDistance(calRssi, rssi, n)` to get the
     expected meters.
4. **Add** the row(s):
   - Append to `formula_fixtures.csv` and/or `filter_fixtures.csv`
     here in this directory (human-readable).
   - Mirror into `test/test_native_distance/fixtures.h` (the array the
     test binary actually reads).
5. **Set tolerance** snug:
   - Formula rows: `0.005 m` is fine for sub-1 m, scale with distance
     (e.g. `0.1 m` at 10 m). Tighter than ±5% is over-fit.
   - Filter rows: `0.5 dB` for steady streams, `1.0 dB` for noisy ones.
6. **Run** `pio test -e native --filter test_native_distance` and
   confirm only your new row(s) changed status.
7. **Open** a PR titled `fixture: <board> <distance>m <beacon-type>`.
   Include a link to the raw capture in the PR description.

## Tier-1 coverage status

QA kickoff baseline (2026-04-26). Formula rows cover all four tier-1
boards because the math is board-agnostic; the rows differ in which
calRssi is realistic per chip's RX sensitivity. **Real-RSSI captures
are not yet in the corpus** — contributions tracking real captures
welcome.

| board     | formula rows | filter rows | real-RSSI capture? |
|-----------|--------------|-------------|--------------------|
| esp32     | 4            | shared      | none yet           |
| esp32c3   | 3            | shared      | none yet           |
| esp32s3   | 3            | shared      | none yet           |
| esp32c6   | 3            | shared      | none yet           |
| `_any`    | 2            | shared      | n/a                |

Filter rows are board-agnostic (synthetic streams). Once captures
arrive they get split per-board. The `_any` rows test the
absorption-guard math itself (not a board calibration).

## Open follow-ups

- **#1817 (wrong room despite stable coordinates)** — the symptom is
  trilateration / room-allocation choosing the wrong room while the
  per-station distance estimates themselves are stable. That logic
  lives in the Companion app (Python), not the firmware. **A native
  distance-calc fixture for #1817 is not reproducible from the issue
  data** because the firmware-side numbers are reportedly correct.
  Filed as ESPA-24 follow-up: capture a Companion-side fixture from
  the user's MQTT trace once available.
- **#1384 (distance reporting problems)** — sparse / dropping distance
  reports. Could be rolling-window starvation
  (`AdaptivePercentileRSSI` returns 0 when the window goes empty).
  Add a fixture once we have a captured stream that reproduces.
- **Per-board real captures** — every tier-1 variant should ship at
  least one capture-derived fixture before the trilateration-accuracy
  harness goes live. Coordinated with the hardware test matrix
  (ESPA-21).
