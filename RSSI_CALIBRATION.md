# RSSI calibration fixes

Status: plan. No firmware, UI or Companion behavior changes in this PR; each fix below lands as
its own PR with tests.

Calibration today applies the receiver adjustment twice in one place, mixes old and new
adjustments in another, and gives every ESP32-S3 a +20 dB default that wasn't measured per
board. This plan fixes those bugs without a compatibility mode: the meaning of the settings
stays the same, so values people have already fitted stay valid.

## The model

One model, used everywhere:

```text
normalized_rssi = raw_rssi - rx_adj_rssi          (rx_adj_rssi: receiver bias in dB)
distance        = 10 ^ ((rssi_1m - normalized_rssi) / (10 * absorption))
```

`rx_adj_rssi` keeps its sign: positive means this receiver reads stronger than the reference
receiver. That is what the firmware and the Companion already do, so retained settings and
fitted values keep their meaning. Example: with a 1 m reference of -65 dBm, a node reading -60
needs +5 and a node reading -70 needs -5; both then normalize to -65 and report 1 m.

The bias is subtracted exactly once. Nothing adds it back to a reference or a threshold.

## Fixes

| # | Bug | Evidence (main) | Fix |
| --- | --- | --- | --- |
| 1 | The Companion sends 0 as an empty payload, which the node reads as reset-to-default: +20 on S3. An S3 fitted to 0 got +20. | ESPresense-companion `MqttCoordinatorExtensions.UpdateSetting` | Send `"0"`; reset calibration with explicit empty payloads. **ESPresense-companion#1765.** |
| 2 | Close/left applies the bias twice: `rssi` is already normalized, then compared with `-40 + rxAdjRssi` / `-50 + rxAdjRssi`. With the S3's +20, "close" needs a raw reading above 0 dBm, so it never triggers. | `BleFingerprint.cpp`, close/left check | Compare normalized RSSI with -40 (enter) and -50 (leave). Keep the hysteresis. |
| 3 | The smoothing window stores already-adjusted samples, so changing `rx_adj_rssi` live mixes two calibrations until the window refills. | `BleFingerprint::seen()`: `addMeasurement(raw - rxAdjRssi)` | Store raw samples; subtract the current bias when reading the filtered value. Distance variance uses the raw window with `rssi_1m + bias`, the same thing in the raw coordinate system. No second filter. |
| 4 | 1 m references are `int8_t` and sums can wrap: an Eddystone power of -100 gives -141, which wraps to +115. `-128` doubles as "missing". | `BleFingerprint.h`: `calRssi`, `bcnRssi`, `mdRssi`, `asRssi` | 16-bit references with a separate "missing" flag; compute sums in wide arithmetic. |
| 5 | Changing `tx_ref_rssi` doesn't change what the node advertises: the iBeacon payload is built once at setup. | `Enrollment.cpp`: `beaconAdv` | Rebuild the payload on the BLE task when `tx_ref_rssi` changes, and re-advertise (outside enrollment mode). |
| 6 | Every ESP32-S3 gets `rx_adj_rssi` +20 by default, whatever the board or antenna. | `defaults.h`, from #1571 | Default 0 on every chip. Per-board offsets move to board templates (below). |
| 7 | The calibration guide describes the adjustment as additive, the opposite of the firmware, and says more absorption gives longer distances (it gives shorter ones for signals weaker than the 1 m reference). | espresense.com/guides/calibration | Correct both, in the documentation repository. |

Fixes 2-5 are plain bug fixes and apply unconditionally: nobody needs the old behavior back.

## Per-board offsets live in templates

A receiver's offset is a property of the board and its antenna, not of the chip. Board templates
already carry a board's pins and settings, and they can set any setting, including
`rx_adj_rssi`. So:

- The firmware default for `rx_adj_rssi` is 0 on every chip.
- A board measured to read hot or cold carries its offset in its template on espresense.com,
  e.g. `"rx_adj_rssi": 8`. Boards without a measurement leave it out.
- The Companion's fitted per-node values still override the template, as any explicit setting
  does.

**Keeping existing S3 calibrations, without a mode switch.** On the first boot of the release
with fix 6, an ESP32-S3 that has WiFi configured and no stored `rx_adj_rssi` stores 20
explicitly, so it keeps behaving as before. A freshly installed node has no WiFi configured yet,
so it starts at 0 and gets its offset from its template. An explicitly stored value, from the
user or the Companion, is never touched.

**Measuring.** The HIL bench has four boards and a BLE flood transmitter in fixed positions.
A HIL step that records each board's smoothed RSSI of the flood dongle gives a measured offset
per board relative to the others; repeated over runs, it also shows how stable that offset is.
Offsets go into templates only from measurements like these, with the board, the antenna and the
reference noted in the template's page.

## Out of scope for now

These were in the first draft. They can follow if someone needs them:

- New report fields (`rssiRaw`, reference provenance) and a separate fallback-reference setting.
- Changing reference precedence (advertised transmit power over a model's estimate).
- A Legacy/Modern mode switch. Fixes 2-5 don't need one, and fix 6 is handled by pinning the
  old default on existing S3 nodes.

Companion follow-up, to check separately: the first draft reported that `RxAdjRssiOptimizer`
uses a fixed -59 dBm reference and applies the candidate bias to RSSI that is already
normalized.

## Tests

Host tests for each firmware fix, using these values:

- **Normalization:** reference -65, raw -60, bias +5 gives normalized -65 and 1 m; raw -70,
  bias -5 gives the same. At absorption 2, normalized -75 gives 3.1622777 m and -55 gives
  0.31622777 m.
- **Close/left:** enter above normalized -40, leave below -50, keep the state in between and at
  both boundaries, with positive and negative bias.
- **Live bias change:** with a full window, changing the bias moves the whole window on the next
  advertisement; RSSI variance is unchanged. Raw samples -65/-75, bias +5, reference -65,
  absorption 2 give per-sample distances 1.7782794/5.6234133 m and variance 3.6962636 m².
- **References:** Eddystone -100 gives -141 dBm at 1 m; sums at the input limits don't wrap;
  missing stays distinct from any value.
- **S3 default:** an S3 with WiFi configured and no stored bias pins 20; a fresh S3 gets 0; a
  stored value is never changed.
- **Advertisement:** after a `tx_ref_rssi` change, the next iBeacon payload carries the new value.

Fix 3 changes the hot path, so it also needs the long HIL soak (`[soak]` in the PR title) with
heap compared against main.
