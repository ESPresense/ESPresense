# RSSI calibration specification

Status: implementation specification. Firmware and UI behavior have not been
changed. This document specifies the complete improvement, including the
compatibility switch, without claiming implementation or hardware validation.

## Recommended model

Keep the existing subtraction convention for `rx_adj_rssi`, and describe it as
**receiver bias (dB)**. Positive means this receiver reads stronger than the
reference receiver; negative means it reads weaker. This matches firmware and
Companion today and avoids changing the meaning of persisted and retained values.

Use one model throughout:

```text
raw_rssi = smoothed measurements from the radio
normalized_rssi = raw_rssi - receiver_bias
reference_1m = expected normalized RSSI from this transmitter at 1 meter
distance = 10 ^ ((reference_1m - normalized_rssi) / (10 * path_loss_exponent))
```

Equivalently, expected raw RSSI at 1 m is `reference_1m + receiver_bias`.
Never add receiver bias to the reference AND subtract it from measurements.

For a reference of -65 dBm, a receiver reading -60 dBm needs bias +5 dB;
a receiver reading -70 dBm needs bias -5 dB. Both then normalize to -65 dBm
and report 1 m. The subtraction is coherent; the existing wording is ambiguous.

Smooth raw measurements and apply the current bias afterward. RSSI variance is
unchanged by a constant bias. Distance variance must use that same current bias;
with raw samples, pass `reference_1m + receiver_bias` to the existing distance
variance routine. This keeps the complete window in one coordinate system.

## Confirmed firmware findings

| Finding | Evidence | Proposed fix |
| --- | --- | --- |
| Bias is applied twice for close/left detection. | `main/BleFingerprint.cpp`, `seen()` subtracts `rxAdjRssi`, while `shouldCount()` adds it to the thresholds. | Compare normalized RSSI against fixed -40/-50 dBm thresholds once. Distance based counting already uses `dist` separately. |
| Live adjustment changes mix calibration states. | `seen()` stores adjusted measurements; `BleFingerprintCollection::Command()` changes bias without transforming existing samples. | Store raw samples, then normalize using the current bias. |
| Reports expose normalized RSSI without an explicit raw value. | `BleFingerprint::fill()` publishes `rssi` and `rxAdj`. | Preserve those meanings and add `rssiRaw` for the smoothed raw value; optionally expose the latest packet RSSI separately under an explicit name. |
| Node transmit reference changes do not update its advertisement. | `Enrollment::Setup()` builds `beaconAdv` once; the `tx_ref_rssi` command changes only the variable and file. | Rebuild and restart the advertisement on the BLE task when its reference changes, respecting enrollment mode. |
| Some reference arithmetic can wrap. | `asRssi`, `mdRssi`, `bcnRssi` are `int8_t`, but hold sums such as `ref_rssi + txPower` and Eddystone power minus 41. | Use wider intermediates and reference storage, and validate applicable ranges before use. Preserve a distinct missing-value representation. |
| S3 gets an unexplained universal bias. | `main/defaults.h` gives all `ESP32S3` builds +20. History traces this to PR #1571; current code does not distinguish antenna/board measurements. | Preserve for Legacy; require measured board-specific defaults for modern mode, otherwise use zero. Switching needs explicit recalibration if this default was in use. |

Changing receiver normalization should also explicitly decide whether BLE query
thresholds use radio RSSI or normalized RSSI. Recommend smoothed raw RSSI for link
quality decisions (connect/battery-query gating), and normalized RSSI for proximity.
Today both consume the same adjusted `rssi` member.

## Reference values: distinguish estimates from measurements

The existing precedence is explicit device calibration, beacon reference,
manufacturer estimate, service/advertisement estimate, then a generic fallback.
That precedence is broadly sensible; the issue is hiding the source and assumptions.

- `rssi@1m` is a measured/declared received-power reference, in dBm.
- BLE AD transmit power is radio output power, in dBm. Adding that power to a
  baseline received RSSI for a 0 dBm transmitter is dimensionally sensible as an
  estimate. It does not establish the transmitter's antenna efficiency or actual
  calibration. The arithmetic itself is not a bug.
- `ref_rssi` is the normalized 1 m baseline for a 0 dBm transmitter, not a second
  receiver correction. It should be common to the reference system, rather than
  tuned per receiver alongside bias.
- `tx_ref_rssi` describes this node's outgoing iBeacon reference. It does not set
  the radio's physical transmit power.
- Eddystone's minus-41 conversion is specified by its protocol, not an arbitrary
  receiver adjustment; retain it.

Expose the reference sources defined below with the report/UI. Label the existing model-specific
power constants as estimates until measurements justify replacing them. Make the
generic fallback explicit: it is currently `ref_rssi - 6` (default -71 dBm), not
the -65 dBm shown for `ref_rssi`. An explicit fallback RSSI@1m setting can preserve
that default while eliminating the hidden assumption of -6 dBm transmit power.

Calibration of a device's 1 m reference should use normalized RSSI from an anchored
receiver. Using a biased receiver's raw RSSI as the reference and then correcting
that receiver again absorbs its hardware bias twice.

## Companion and documentation findings

Companion's `src/Models/OptimizationSnapshot.cs`, `Measure.GetAdjustedRssi()`, uses:

```text
candidate_rssi = reported_rssi + reported_rxAdj - candidate_rxAdj
```

It therefore depends on the existing sign and on `rssi` already being normalized.
Silently changing `rssi` to raw, or reversing the meaning of `rxAdj`, breaks
optimization. Keeping these fields stable and adding raw RSSI is compatible.

The local Companion checkout also has two issues relevant to this work:

- `RxAdjRssiOptimizer` uses a hard-coded -59 reference and adds the candidate bias
  to RSSI that is already corrected, without undoing the reported old bias. It
  should use the measurement/reference and the shared candidate conversion.
- `MqttCoordinatorExtensions.UpdateSetting()` turns integer zero into an empty
  payload for `rx_adj_rssi`. Firmware interprets empty as reset to the default,
  which is +20 on S3. Zero and reset need distinct representations.

When fitting transmit references and receiver biases together, the model cannot
determine their absolute levels from node-to-node observations alone:
`reference_1m + bias` is invariant if every transmit reference increases by k
and every receive bias decreases by k. Anchor a measured reference receiver at
zero and/or fix a calibrated transmitter reference. Bounds limit drift but do
not supply a physical calibration anchor.

The published calibration guide describes an additive receive correction,
opposite to firmware and Companion, and says increasing absorption increases
distance. For signals weaker than the 1 m reference, the actual equation gives
smaller distances as the exponent increases. Both descriptions need correction.

## Required settings and defaults

| Key | Type / range | Legacy default | Modern default | Meaning |
| --- | --- | --- | --- | --- |
| `legacy` | Boolean | `true` when absent | Explicit `false` selects modern | Preserve historical calibration behavior. |
| `rx_adj_rssi` | Integer dB, -100 to 100 | Existing board default (S3 +20, others 0) | 0 | Receiver bias, subtracted once for normalization. Explicit stored values always win. |
| `ref_rssi` | Integer dBm, -100 to 100 | -65 | -65 | Baseline at 1 m for a 0 dBm transmitter; used only for estimates. Keep existing range for compatibility. |
| `fallback_rssi_1m` | Integer dBm, -150 to 0 | Unused | -71 | Explicit reference for devices lacking calibration, usable transmit power, or a model estimate. |
| `tx_ref_rssi` | Integer dBm, -100 to 0 | -59 | -59 | Calibrated reference advertised by this node; never changes physical radio power. |
| `absorption` | Finite float, 1 to 5 | 2.7 | 2.7 | Path-loss exponent; must be positive. |

Missing `legacy` means ON for both upgrades and fresh installations in this
release. This avoids needing an unreliable first-install detector. Modern mode
is explicit opt-in. Future default changes are outside this specification.

An empty MQTT calibration payload resets that setting to its active mode's
default. Literal `0` means zero wherever zero is valid. Do not conflate the two.
An unchecked HTML checkbox must store false, not delete the mode setting.
Reject malformed, nonfinite, and out-of-range inputs without changing the last
valid setting; report the rejected key/value. Apply identical validation to
HTTP, MQTT, board-template application, and persisted values at startup. Invalid
persisted values use the mode default and emit a diagnostic. Successful writes
must precede activation; failed storage must not create a runtime-only change.

## Exact Legacy behavior

| Operation | Legacy ON | Legacy OFF |
| --- | --- | --- |
| Filter samples | Store `packet_rssi - bias_at_sample_time`. | Store unmodified packet RSSI. |
| Published `rssi` | Existing filtered adjusted RSSI. | Filtered raw RSSI minus current applied bias. |
| Distance | Existing reference minus filtered adjusted RSSI. | Selected reference minus normalized RSSI. |
| Close/left | Existing adjusted RSSI compared with -40/-50 plus bias. | Normalized RSSI compared with fixed -40/-50 thresholds. |
| Query/battery link thresholds | Existing adjusted RSSI. | Smoothed raw RSSI, independent of calibration. |
| Reference selection | Existing precedence and power assumptions. | Explicit precedence below, with reference provenance. |
| Unset receiver bias | Existing board default. | Zero; no universal S3 offset. |
| Generic reference | `ref_rssi - 6`. | `fallback_rssi_1m`. |

Legacy protects historical numeric behavior for valid settings. It does not
preserve overflow, malformed settings, stale advertisements, or missing-reference
sentinel collisions. Fix those defects in both modes. Static settings with a
nonoverflowing reference must retain Legacy distances and existing report fields
within the current single-precision tolerance.

Modern mode retains the existing smoothing estimator (Tukey-fenced mean despite
the method name `getMedianIQR`), time window, reporting schedule, and path-loss
equation. This work does not replace the filter or introduce another distance
curve. Proximity remains hysteretic: enter only above -40 dBm, leave only below
-50 dBm, and retain the previous state at the boundaries/in between.

## Modern reference resolution

Resolve references in this order on each measurement update:

1. Explicit per-device `rssi@1m` calibration.
2. Valid iBeacon/AltBeacon 1 m reference, or Eddystone reference converted by -41.
3. Usable BLE AD transmit power plus `ref_rssi`.
4. Existing recognized model's estimated transmit power plus `ref_rssi`.
5. `fallback_rssi_1m`.

This deliberately lets advertised transmit power take precedence over a model's
hard-coded power estimate in modern mode. Legacy retains its existing precedence.
Do not infer a calibrated 1 m reference from the numerical appearance of an AD
transmit-power field; use its protocol meaning. Keep each selected source and its
underlying data separate from the derived reference so changing `ref_rssi` takes
effect on the next measurement without requiring rediscovery of the device.
Removing a device override must clear it and restore the next available source.

Use signed storage of at least 16 bits for references and sums, with an explicit
valid flag (or optional value), not -128 as a universal missing sentinel. A valid
Eddystone value of -100 yields -141 dBm at 1 m and must never wrap to +115.
Perform conversions in wide arithmetic. If a derived distance/variance is
nonfinite, omit that quantity from the report and record a diagnostic; never
substitute a plausible-looking finite distance or serialize NaN/Infinity.

## Reporting and update consistency

Keep `rssi`, `rssi@1m`, `rxAdj`, `rssiVar`, `distance`, and `var` with their
existing units and meanings. Publish finite valid references/normalized RSSI
even below -128; do not reuse the historical sentinel comparison to hide them.
In modern mode add:

| Field | Meaning |
| --- | --- |
| `rssiRaw` | Smoothed raw radio RSSI, in dBm, over the same window as `rssi`. |
| `rssiSource` | Reference provenance: `device`, `beacon`, `tx-power-estimate`, `model-estimate`, or `fallback`. |
| `legacy` | Boolean indicating the active calculation mode. Include this field in both modes. |

Do not report Legacy `rssi + current bias` as raw RSSI: historical mixed-bias
windows do not contain enough information to recover the true raw statistic.
Omit `rssiRaw` in Legacy. Do not allocate a second filter just to produce it.

Modern reports must satisfy `rssiRaw - rxAdj = rssi` within serialization precision.
Take one calibration snapshot for each measurement calculation. Publish its bias,
reference, mode, RSSI, distance, and variances together, rather than combining
cached RSSI with a newly changed global bias/reference. Before another advert
arrives, a report may still contain the previous complete snapshot. Settings
telemetry may already advertise the new setting; these describe different times.

Live bias, reference, and absorption changes take effect on the next measurement
in modern mode; all existing raw samples use the new calibration together. The
same principle applies to distance variance. Keep filter calls on the BLE host
task because its existing shared scratch buffer assumes a single caller.
Protect calibration snapshot reads and fingerprint publication against concurrent
MQTT/HTTP changes. Respect the existing lock order; never mutate filter buffers
from the settings or MQTT tasks. Copies/aliases created by `setInitial()` must
carry the raw filter representation and complete snapshot, not just RSSI/distance.

Publish retained room settings `legacy` (`ON`/`OFF`), `ref_rssi`, and
`fallback_rssi_1m` alongside the existing calibration settings. Device reports are
additive schema extensions; no consumer must infer mode from firmware version.

## UI and migration

Under Calibration, show a checkbox labeled **Legacy** with help text:
"Preserve historical RSSI calibration behavior. Uncheck to apply receiver bias
once and use explicit reference estimates. Saving restarts this node."

Rename the receiver label to **Receiver bias (dB)**, with examples:
"+5 if this node reads 5 dB too strong; -5 if it reads 5 dB too weak."
Show **Fallback RSSI at 1 m (dBm)** only in modern mode. Retain `ref_rssi` and
label it **Estimated RSSI at 1 m for a 0 dBm transmitter**; label `tx_ref_rssi`
**This node's advertised RSSI at 1 m**. Explain that increasing absorption gives
smaller distances for signals weaker than the reference. Device/fingerprint
tables distinguish raw and normalized RSSI and display reference provenance.

Toggling the checkbox must not alter any displayed numeric values. On Save,
persist the displayed effective receiver bias as an explicit value along with
the mode, so an unset S3 +20 does not silently become zero. If no modern fallback
was previously stored, initialize it to the old generic effective reference
`ref_rssi - 6`, rather than blindly using -71 on a customized node. Show this
value before saving. This is a one-time migration initialization, not a permanent
link between the two settings. Preserve the modern fallback when returning to
Legacy, although Legacy does not use it.

Thereafter, Reset receiver bias uses the active mode default: zero in modern
mode. No automatic sign reversal or fleet recalibration occurs. Explain that
close/left and references based on advertised versus estimated power may change
even if the numeric bias is preserved; Legacy can be re-enabled to restore those
historical choices. Full form submissions must include hidden/preserved values
for settings outside the selected mode because this endpoint resets omitted keys.

Mode changes take effect at restart. HTTP Save uses the existing save/restart
flow. MQTT `legacy/set` accepts `ON` or `OFF` (empty resets to ON), rejects other
values, and schedules a restart after successful persistence when the mode
changes. Before this restart, materialize an unset receiver bias and initialize
an unset modern fallback as above. An unchanged mode does not restart. After
boot, the retained mode status confirms which mode is active. The UI must check
the Save response status and display validation/storage failures; it must not
restart after a rejected save. Apply the same
preservation rule when a board template changes the mode; do not rely on the UI
alone to perform migration. Persist required numeric values before changing the
mode so an interrupted write cannot boot the new mode with unintended defaults.

An additive correction setting with the opposite sign is not part of this
specification. The existing subtraction convention is coherent and preserves
retained commands and Companion compatibility.

## Advertisement and Companion changes

On a successful `tx_ref_rssi` update, the BLE task must rebuild the cached payload
and re-advertise it by its next loop iteration while normal iBeacon advertising
is active. During enrollment, rebuild the cache but retain connectable advertising;
the next return to iBeacon mode uses the new reference. Do not call BLE advertising
operations from MQTT callbacks. The advertised signed byte, retained room setting,
and configured reference must agree after the update completes.

Companion must keep the current bias convention, send literal zero separately
from reset, and use the shared candidate conversion and selected reference in
every optimizer. Prefer `rssiRaw - candidate_bias` when modern raw RSSI is present;
otherwise use `rssi + reported_bias - candidate_bias`. Historical Legacy windows
with mixed sample biases cannot be made exact by that inverse: exclude snapshots
spanning adjustment changes until a full smoothing window has elapsed. Add a
measured calibration anchor when jointly fitting transmitter references and
receiver biases; keep that anchor fixed and report it in calibration results.

Update the calibration/MQTT/optimization documentation together with the
Companion change. Firmware can ship its compatible report extensions first;
the corrected optimization experience requires the Companion fixes. Do not
claim that selecting modern mode alone repairs previously fitted values.

## Acceptance criteria

These are requirements for the implementation, not completed test results.

1. **Normalization:** reference -65, raw -60, bias +5 gives normalized -65 and
   distance 1 m; raw -70, bias -5 gives the same. Zero bias leaves RSSI unchanged.
   At exponent 2, normalized -75 gives 3.1622777 m and -55 gives 0.31622777 m.
2. **Proximity:** in modern mode, enter above -40, leave below -50, and retain
   state at either exact boundary. Test positive and negative biases. Legacy
   still uses its historical thresholds; S3 bias +20 implies raw enter above
   0 dBm in Legacy versus above -20 dBm in modern mode.
3. **Filter/update consistency:** with a fixed reference, changing bias while
   a window is populated immediately corrects the entire window on the next
   advert in modern mode. No sample-history reset or second filter is needed.
   Verify RSSI variance is unchanged, snapshot fields remain coherent under
   concurrent reports/commands, and copied fingerprints obey the same invariant.
4. **Distance variance:** raw samples -65/-75, bias +5, reference -65, exponent
   2 produce per-sample distances 1.7782794/5.6234133 m and population variance
   3.6962636 m squared. Passing `reference + bias` with raw samples must equal
   the manually normalized calculation. Use the existing window/sample policy.
5. **References:** exercise every precedence level, including advertised power
   versus a conflicting model estimate. Clear a device override and observe
   the next source. Changing baseline affects estimated references on the next
   measurement, but not measured beacon/device references or the explicit fallback.
6. **Arithmetic/validation:** Eddystone -100 converts to -141 without wrapping.
   Test sums at allowed input extremes, normalized RSSI below -128, valid zero,
   malformed integers, nonfinite floats, and out-of-range inputs at every write
   entry point. Rejected values preserve active settings; storage failures do
   not activate changes; nonfinite derived values never reach JSON.
7. **Migration:** boot with no mode file stays Legacy. Test unconfigured and
   explicitly configured S3 bias, non-S3 bias, a customized baseline, mode
   round trips, and interrupted persistence. Switching an unset S3 to modern
   pins +20; later resetting bias selects 0. A baseline -70 initializes a
   previously unset modern fallback to -76, which remains independent afterward.
   Explicit numeric values sent in the same update override migration defaults.
8. **API/UI:** checked and unchecked mode values survive Save/restart; hidden
   fallback values survive Legacy saves; invalid saves show an error and do
   not restart. MQTT unchanged mode does not reboot, changed mode reboots once,
   and status after boot matches the device-report mode. Modern packets satisfy
   `rssiRaw - rxAdj = rssi` within 0.02 dB (two-decimal serialization). Legacy
   packets omit `rssiRaw`; unknown additive fields do not break old Companion.
9. **Advertisement:** update `tx_ref_rssi` during normal advertising and during
   enrollment. Decode the next active iBeacon payload and verify its reference
   agrees with settings. Enrollment advertising must remain connectable until
   the existing enrollment flow exits.
10. **Companion:** literal zero remains zero on S3; reset is separate. With
    reported normalized -70, old bias +5 and candidate bias +2, all optimizers
    use candidate normalized -67. With modern raw -65, candidate +2 gives the
    same result. A calibrated anchor remains fixed during joint optimization.
    Reject Legacy mixed-bias snapshots until a complete window has elapsed.
11. **Regression/hardware:** compare valid static-setting Legacy outputs with
    the existing implementation; use relative distance tolerance 1e-4. Run the
    complete host suite, regenerate UI headers, and build representative ESP32,
    S3, and C3 variants. Measure a fixed beacon at 1 m and a second known distance
    on multiple boards with controlled orientation; confirm bias calibration
    makes their normalized 1 m readings agree within the measured RSSI variability.
    Record measurements instead of promising accuracy from synthetic tests alone.
12. **Stability:** record heap/latency/report-size differences and run the long
    HIL soak required for the filter/stability change. Use a `soak/<name>` branch
    or `[soak]` PR title, inspect the latest CrowCI pipeline, and preserve the
    device logs/result. A 180-second PR run is insufficient evidence for release.

## Implementation boundaries and delivery

Implement the firmware/UI change in this repository, Companion fixes in its
repository, and calibration documentation in the documentation repository.
No existing sensor inputs, RF output settings, enrollment identities, or MQTT
device IDs change as a consequence of mode selection. This is calibration
consistency work; it does not promise to remove multipath/orientation effects.

The firmware work touches `main/defaults.h`, settings/command registration in
`BleFingerprintCollection`, reference/filter/snapshot handling in `BleFingerprint`,
`Enrollment` advertisement refresh, retained settings in `main.cpp`, and the
Settings/fingerprint/device UI and types. Keep the pure math/reference resolution
host-testable. Avoid extra filter allocations and preserve the existing BLE task's
ownership of the rolling buffer. Source provenance must be stored alongside data
when fingerprint parsers discover it, rather than reconstructed from a numeric
reference later.

For implementation validation, source ESP-IDF v5.4 and run `./test/run.sh`;
run `npm run build` in `ui` to regenerate `main` headers after UI edits; build with
`./build.sh <env>` as described in BUILDING.md. Flash verification, if undertaken,
uses `tools/flash.sh <env> <port> [seconds]`, includes the post-flash serial output,
confirms boot, and records the IP. No flash/build/soak is required to complete this
specification alone; those gates apply to the future implementation.

Numeric examples were checked during this investigation. Firmware behavior and
hardware accuracy of the proposed implementation remain unvalidated.

## External sources

- [Calibration guide](https://espresense.com/guides/calibration/)
- [Eddystone-UID specification](https://github.com/google/eddystone/blob/master/eddystone-uid/README.md)
- [Original S3 default change, PR #1571](https://github.com/ESPresense/ESPresense/pull/1571)

Firmware and Companion findings above come from the local checkouts inspected
during this investigation, not from documentation assumptions.
