# mmWave support consolidation audit

Tracks: #1310, #1274, #1191, #860, #676, #679

## Current implementation (as of 2026-02-16)

ESPresense currently supports **radar as a generic digital motion input**:

- Config keys: `radar_type`, `radar_pin`, `radar_timeout`
- Runtime behavior: samples a GPIO pin and publishes `rooms/<node>/radar` as `ON`/`OFF`
- Motion merge: publishes `rooms/<node>/motion` as OR of PIR + radar

Code references:

- `src/Motion.cpp` (`radarLoop`, `ConnectToWifi`, MQTT publish)
- `ui/src/routes/hardware/+page.svelte` (Radar pin UI)

## What this means for open mmWave issues

### Already served by existing generic GPIO support

- #676 DF Robot mmWave (when module exposes digital OUT)
- #860 LD2410 BLE (GPIO out mode only)
- #1191 HLK-LD1125H (GPIO out mode only)

### Not served by current implementation

Needs protocol-specific integration beyond GPIO:

- #1274 LD2450 (UART frames, multiple targets/zones)
- #679 serial ports + LD1115H (UART integration + config channel)
- #1310 SEN0395 settings (sensitivity/LED tuning commands)

## Consolidation direction

To avoid one-off integrations per radar model, split mmWave support into two layers:

1. **Baseline motion layer (already present)**
   - Generic digital `radar_pin` -> `radar` + `motion`
2. **Advanced transport layer (missing)**
   - UART/BLE protocol adapters per sensor family
   - Standardized outputs (e.g. target count, zones, still/moving)
   - Optional control channel (sensitivity, LED, thresholds)

## Proposed next implementation slices

1. Clarify docs/UI that current radar support is digital input only.
2. Add an internal mmWave capability model (`binary_presence`, `target_count`, `zones`, `device_control`) to drive discovery/UI consistently.
3. Add first serial adapter behind feature flag (LD2450 or LD1115H), mapping to normalized capability topics.

## Concrete outcome from this step

This document captures issue triage and a shared architecture baseline so future mmWave work can be grouped under one model instead of scattered per sensor request.
