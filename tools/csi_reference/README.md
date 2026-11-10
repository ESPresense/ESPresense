# Reference detector benchmark

This is a research build for the fixed **Family** and **Master** nodes. It does not deliver an occupancy feature. Compare the unchanged upstream ESPectre Lightweight and High Accuracy detectors with the earlier sparse-stream approximation, then evaluate whether motion plus BLE improves occupancy over BLE alone.

## Source and build

`tools/prepare_csi_reference.py` fetches upstream commit `3edc7bdb4262e01b868c3d589fea59cfd0e8f555`, preserves its license notices, and records the archive hash. The SDK sources, capture, normalization, model weights, calibration, and admission policy are unchanged. The `CSI Reference Benchmark` workflow builds **only** `esp32c3-csiref` under IDF **5.5.6**. Existing firmware variants keep IDF 5.4.4; their CSI implementation and dependencies are unchanged. Image size must fit the existing 1,966,080-byte OTA slot.

Build locally with `python3 tools/prepare_csi_reference.py`, the IDF 5.5.6 environment, and `./build.sh esp32c3-csiref`.

The integration retains ESPresense networking, BLE scanning, configured plug GPIOs, SPIFFS settings, and OTA. It uses the full upstream SDK rather than a locally fitted copy of two features. The SDK generates 100 ICMP requests/s to the associated WiFi gateway, keeps WiFi power save off, and controls its radio policy, including its WiFi coexistence preference. All of those costs belong to the reference result. Association, gateway identity, actual CSI rates, slot coverage, and BLE counters must be recorded rather than inferred from the requested packet rate.

## Bounded operation

The benchmark is off by default. `/wifi/hardware` exposes `csi_reference_mode` (0 off, 1 Lightweight, 2 High Accuracy) and `csi_reference_until` (absolute UTC Unix seconds). They take effect after reboot. The reference cannot arm before NTP establishes a valid clock or with an expired deadline or a deadline more than 48 hours away. Once armed, a monotonic timeout also bounds the trial if wall time changes. Expiry persists mode 0 and restarts to restore ordinary radio policy. Set the first pilot deadline about 15 minutes ahead, not a day ahead.

Before flashing either node, save its current firmware identity, settings, board template, auto-update state, and a verified restoration artifact. Disable auto-update for the experiment. Only use an app image after checking target, size, successful bench boot, and OTA compatibility; never use a factory image over OTA. Do not restart a downloading node. Verify the post-flash boot log and `/json` build before arming. Retain a recorder through timeout and confirm disarm. Restore the original firmware/settings and auto-update state after the trial.

## Measurements and admission gate

Record the node TCP log on port 6053, polling `/json` for BLE counters, uptime, and heap. `CSIREF` emits four snapshots/s with detector identity, monotonic and UTC time, readiness, score, threshold, calibration, accepted/admitted packet counts, temporal slot coverage, queue drops, traffic counters, and heap. An unready interval is **unknown**, never an empty-room prediction. Calibration failures and queue loss stay in the report. The integration registers no HA motion sensor and drives no household automation.

First run a baseline/reference/baseline/reference pilot with equal blocks on a fixed-load bench. Use cumulative **received BLE advertisement** counters as the primary measurement, with counter resets and reboots treated as new epochs. Serial New/Del fingerprint events may be included only as a separate proxy. Verify which `/json` counter represents received advertisements before using it. Measure heap and image budget, and record actual slot coverage: a requested 100 pps without sufficient valid CSI is an acquisition failure, not negative evidence about the detector.

After the pilot establishes stable, valid input, collect matched periods covering a normal day and night per room and detector. Rotate detector order across the rooms and repeat periods rather than confounding mode with one time of day. Evaluate the pre-trained upstream weights and calibration unchanged, without fitting on the evaluation period. Retain gaps and startup intervals in coverage statistics. Do not claim a controlled BLE result from ambient household traffic.

## Map and reference labels

Use the companion's floor polygons and configured node coordinates, not node display names. Family and Master are the primary rooms; both use ESP32-C3 SwitchBot Plug Minis. The Master node is about 0.2 m from its shared bathroom wall; use the Master Bathroom, Closet, adjoining Office, and the floor below as interference checks. Family needs adjacent Kitchen and Laundry activity checked separately. The `laundry` node sits at `(4.3, 7.6, 1.4)` on the Downstairs Hall boundary, outside the Laundry polygon: earlier Laundry-only scoring is not a clean room-level validation. T-Embed moves and is excluded; Office's old M5StickC is not a primary validation node.

Verify the sensor histories and underlying definitions before assigning labels. Family PIR motion establishes positive motion evidence but PIR silence cannot establish absence. Family's aggregate occupancy and presence sensors have disagreed in earlier captures. Master currently exposes `master_bed_presence`, `master_presence` (Aqara FP2), and aggregate `master_occupancy`; inspect their coverage and dependencies rather than treating their names or one current state as truth. Do not use BLE-derived reference labels to validate BLE fusion. Keep uncertain and unavailable intervals explicitly unlabelled.

## Product evidence required

Report motion detection and occupancy as different outcomes. Score arrivals, retained occupancy while still, departures, a person without a tracked device, and activity in neighboring rooms. Compare CSI plus BLE with BLE alone on the same evaluation intervals. Report per-room recall, precision, arrival/departure latency, false occupancy episodes/hour, false occupancy duration, missed occupancy duration, room flapping, and the fraction of time with valid labels and ready input. AUC and false-positive windows/hour alone cannot establish useful occupancy. Preserve a reproducible negative result if neither detector helps; green CI or a merge does not satisfy the outcome.
