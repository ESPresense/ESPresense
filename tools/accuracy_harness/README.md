# Accuracy harness

Simulated, RSSI-driven trilateration accuracy harness for ESPresense.
Produces the public baseline numbers in
[`docs/accuracy.md`](../../docs/accuracy.md). Pure-Python stdlib, no
external dependencies. Owned by the QA Engineer.

## Run

From the firmware repo root:

```bash
python3 -m tools.accuracy_harness baseline \
  --output tools/accuracy_harness/reports/baseline-v1.json \
  --markdown tools/accuracy_harness/reports/baseline-v1.md
```

Useful flags:

```
--seed                       deterministic RNG seed (default 20260426)
--gaussian-std-db            RSSI noise sigma in dB    (default 2.5)
--multipath-probability      per-sample multipath prob (default 0.10)
--multipath-attenuation-db   multipath dB hit          (default 6.0)
--scenarios FILE [FILE ...]  scenario JSONs under scenarios/
```

## Tests

```bash
python3 -m unittest discover -s tools/accuracy_harness/tests -t .
```

The firmware-formula parity tests will fail if anyone changes
`lib/rssi_distance/rssi_distance.h::rssiToLogDistance` without
re-baselining. That is intentional.

## Layout

| File | Purpose |
|---|---|
| `firmware_model.py` | Mirror of the firmware log-distance formula. Distance ↔ RSSI helpers. |
| `geometry.py`       | Scenario / Room / Station / Track loaders. |
| `noise.py`          | RSSI-side Gaussian + multipath-burst noise model. |
| `solver.py`         | Pure-stdlib Nelder-Mead reference solver (v1 stub; replaced by Companion CLI when available). |
| `runner.py`         | Per-scenario / per-track execution + metric scoring. |
| `report.py`         | Versioned JSON + Markdown rendering. |
| `__main__.py`       | `python -m tools.accuracy_harness` CLI. |
| `scenarios/*.json`  | Room geometry + ground-truth tracks. |
| `reports/*`         | Checked-in baseline snapshots. |
| `tests/`            | unittest tests for parity, solver convergence, determinism. |

## Adding a scenario

Copy an existing JSON in `scenarios/`, change the bounds / rooms /
stations / tracks, and run:

```bash
python3 -m tools.accuracy_harness baseline --scenarios your_scenario.json
```

Open a PR with the scenario file plus the report it produced.

## v1 limitations

See [`docs/accuracy.md`](../../docs/accuracy.md) §"What this v1 does NOT
model" for the full list. The big ones:

- Reference solver, not Companion's actual `ILocate` (tracked
  Companion-side; will swap when CLI ships).
- 2D only. No Kalman. No per-board RSSI variance modeling.
