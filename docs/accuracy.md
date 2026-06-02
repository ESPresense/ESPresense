# How accurate is ESPresense?

**Short answer (v1 baseline, June 2026):** on a tier-1 4-node ESP32 setup in a
6m × 4m two-room floor, simulated positioning lands at a **median error of
~0.60m** and **p95 of ~1.3–1.6m**, using the real Companion `ILocate`
solver. Room mis-identification is **~6.6%** in normal walking and rises near
room boundaries (by design — see the canonical doc).

> These figures are a **frozen snapshot** of the first published baseline, kept
> here so the firmware repo answers the "how accurate is this?" question
> directly. They are **not** the live source of truth and may lag. For the
> current numbers, the full methodology, the regression-tested CI guard, and
> the documented blind spots, always cite the canonical baseline:
>
> **→ [ESPresense-companion `docs/accuracy.md`](https://github.com/ESPresense/ESPresense-companion/blob/main/docs/accuracy.md)**

## Why accuracy lives in the Companion repo, not here

The firmware on each ESP32 node measures **distance** to a Bluetooth device
(RSSI → meters via the log-distance formula) and publishes it over MQTT. It
does **not** compute position. Trilateration — turning several nodes' distance
streams into an (x, y) location and a room assignment — runs in the
[ESPresense-companion](https://github.com/ESPresense/ESPresense-companion) app's
`ILocate` implementations (`NelderMead`, `GaussNewton`, `BFGS`, `MLE`).

Positioning accuracy is therefore measured and published where the positioning
code actually lives. The accuracy harness drives the **real** `ILocate` against
seeded, reproducible scenarios and commits a versioned baseline; a CI guard
re-runs it on every PR that touches the locator code path and fails on drift
outside a ±5% tolerance.

## What the firmware repo *does* own

The piece of the accuracy chain that lives here is the **RSSI → distance**
conversion (`d = 10^((P0 - RSSI) / (10 · n))`):

- `src/BleFingerprint.cpp` — applies the log-distance formula per fingerprint.
- `lib/filtering/AdaptivePercentileRSSI.cpp` — RSSI smoothing and the
  `getDistanceVariance` path that feeds it.

A host-side (`native`) unit-test suite with known-distance fixtures for this
conversion is being added under `test/`; see
[#2317](https://github.com/ESPresense/ESPresense/pull/2317).

If the distance numbers a node reports are wrong, that is a firmware bug and
belongs to the fixtures above. If the *position* is wrong while the distances
are stable (e.g.
[#1817](https://github.com/ESPresense/ESPresense/issues/1817)), that is a
locator-side concern measured by the Companion harness above.

## Blind spots (v1)

The baseline is **simulated, not measured on hardware**. It deliberately does
not yet model real RF, per-board RSSI variance across tier-1 variants, or
inter-sample smoothing. The full, current list of limitations lives in the
[canonical doc](https://github.com/ESPresense/ESPresense-companion/blob/main/docs/accuracy.md#what-this-baseline-does-not-model-blind-spots).
Real-hardware-in-the-loop measurements will follow in the same report format.
