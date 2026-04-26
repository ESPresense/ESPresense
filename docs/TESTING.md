# ESPresense hardware test matrix

**Status:** Draft v1 — source of truth for the pinned "boards-supported" discussion. Authored by the QA Engineer ([ESPA-22](/ESPA/issues/ESPA-22)). Community Manager pins / republishes ([ESPA-10](/ESPA/issues/ESPA-10)). Tech Lead owns tier-1 designations and the platform-support roadmap ([ESPA-18](/ESPA/issues/ESPA-18)) — if this doc and the roadmap disagree, the roadmap wins and this doc gets corrected.

This file answers two questions the community keeps asking and the project has not had a public answer for:

1. Which boards actually work, and how do you know?
2. What does it mean when a build is in `platformio.ini` but you've never heard anyone confirm it boots?

Snapshot date: **2026-04-26**. Re-reviewed each release. Open a PR or comment on the pinned discussion if a row is wrong.

## Tag legend

| Tag | Meaning |
| --- | --- |
| **tier-1** | A real board of this variant is wired into the Hardware-In-Loop (HIL) rig. Every PR flashes the firmware to that board, brings it up over WiFi via Improv, and runs the HIL monitor. A failure on this board blocks the PR from merging. |
| **tier-1 (soft-fail)** | HIL job exists and runs every PR, but failures are currently `failure: ignore` in `.woodpecker/hil.yml`. Build is exercised on real hardware; runtime regressions will not block merges until the soft-fail is removed. |
| **community-tested** | Not on the HIL rig. A community member has reported a working configuration with name + date + version. We trust the report but cannot detect regressions automatically. |
| **untested** | The `[env:*]` builds in CI but nothing validates runtime behavior — not on the HIL rig and no recent confirmed community reports. May or may not work. |
| **known-broken** | An open issue documents a reproducible failure on this exact variant or sub-variant. Linked. |

## CI coverage

- **Build matrix** (`.github/workflows/build.yml`, line 16): every `[env:*]` is built on every PR. Build success only proves "the binary compiles and fits the OTA partition (1920 KB)". It does **not** prove the firmware boots.
- **HIL matrix** (`.woodpecker/hil.yml`): four boards on the rig — `esp32`, `esp32c3`, `esp32c6`, `esp32s3`. Each PR flashes, joins WiFi via Improv, and runs `hil_monitor.py` for 180 s on PRs, 28 800 s on cron/main. `esp32c6` runs with `failure: ignore` (line 90) — failures don't block. The other three block PRs.
- **Native unit tests** (`pio test -e native`, in CI as `test-native` since [PR #2317](https://github.com/ESPresense/ESPresense/pull/2317)): board-agnostic logic (RSSI → distance, AdaptivePercentileRSSI). 28 cases, fixture corpus in `test/fixtures/`. Does not exercise board-specific BLE / WiFi / display code.
- **Accuracy harness** ([PR #2318](https://github.com/ESPresense/ESPresense/pull/2318), ESPA-21): simulated trilateration regression. Doesn't run on real hardware yet — published numbers are a lower bound. See [`docs/accuracy.md`](./accuracy.md).

## Matrix

### ESP32 family

| `[env:*]` | Reference board | Build CI | HIL | Tag | Notes |
| --- | --- | --- | --- | --- | --- |
| `esp32` | `esp32dev` | ✅ | ✅ blocking | **tier-1** | Flagship. The firmware was originally written for this chip; everything else is a port. ESP32-WROOM / -WROVER consumer boards are the long tail of community-tested hardware here. |
| `esp32-verbose` | `esp32dev` | ✅ | — | debug build of `esp32` | Not a separate board — `CORE_DEBUG_LEVEL=2` + `LOG_LEVEL_DEBUG`. Use only when reproducing a bug. |

### ESP32-C3 family

| `[env:*]` | Reference board | Build CI | HIL | Tag | Notes |
| --- | --- | --- | --- | --- | --- |
| `esp32c3` | `esp32-c3-devkitm-1` | ✅ | ✅ blocking | **tier-1** | HIL-validated. **However**: ESP32-C3-DevKitM-1-N4X consumer hardware has a reproducible boot loop ([#2244](https://github.com/ESPresense/ESPresense/issues/2244)) — see "Known hardware-specific issues" below. Confirm your exact board against [#2252](https://github.com/ESPresense/ESPresense/issues/2252) before flashing. |
| `esp32c3-cdc` | `esp32-c3-devkitm-1` | ✅ | — | **untested** | Same firmware logic as `esp32c3` plus `ARDUINO_USB_MODE=1` / `ARDUINO_USB_CDC_ON_BOOT=1`. No HIL board for this variant; CDC behavior is unverified. |
| `esp32c3-verbose` | `esp32-c3-devkitm-1` | ✅ | — | debug build of `esp32c3` | Same caveats as `esp32-verbose`. |

### ESP32-S3 family

| `[env:*]` | Reference board | Build CI | HIL | Tag | Notes |
| --- | --- | --- | --- | --- | --- |
| `esp32s3` | `dfrobot_firebeetle2_esp32s3` | ✅ | ✅ blocking | **tier-1** | Promoted to blocking in [PR #2272](https://github.com/ESPresense/ESPresense/pull/2272) (April 2026). Earlier S3 builds had been on `failure: ignore`; that toggle is now off. Open S3 user reports: [#2309](https://github.com/ESPresense/ESPresense/issues/2309) (board freeze on 4.1.0b0). |
| `esp32s3-cdc` | `dfrobot_firebeetle2_esp32s3` | ✅ | — | **untested** | CDC variant, same caveat as `esp32c3-cdc`. |
| `esp32s3-verbose` | `dfrobot_firebeetle2_esp32s3` | ✅ | — | debug build of `esp32s3` | |

### ESP32-C6 family

| `[env:*]` | Reference board | Build CI | HIL | Tag | Notes |
| --- | --- | --- | --- | --- | --- |
| `esp32c6` | `esp32-c6-devkitc-1` | ✅ | ⚠ `failure: ignore` | **tier-1 (soft-fail)** | HIL job runs on every PR but failures don't block. Maintainership status — `help wanted` per [ESPA-18](/ESPA/issues/ESPA-18); see [#1354](https://github.com/ESPresense/ESPresense/issues/1354) for Seeed XIAO C6, [#2042](https://github.com/ESPresense/ESPresense/pull/2042) for the original add. Treat as more reliable than community-tested but less reliable than blocking-HIL until soft-fail is removed. |
| `esp32c6-cdc` | `esp32-c6-devkitc-1` | ✅ | — | **untested** | |
| `esp32c6-verbose` | `esp32-c6-devkitc-1` | ✅ | — | debug build of `esp32c6` | |

### M5Stack family

All M5 envs build on every PR but none are on the HIL rig and no recent verified community-tested reports. Historical user activity exists on closed issues (#456, #506, #438, #382) but configurations there are stale. If you run one of these and it works on a current release, please reply on the pinned discussion with version + date so we can promote the row.

| `[env:*]` | Reference board | Build CI | HIL | Tag | Notes |
| --- | --- | --- | --- | --- | --- |
| `m5stickc` | `m5stick-c` | ✅ | — | **untested** | TFT_FONT=1, M5STICK macro. |
| `m5stickc-plus` | `m5stick-c` | ✅ | — | **untested** | TFT_FONT=2, M5STICK + PLUS macros. |
| `m5atom` | `m5stack-atom` | ✅ | — | **untested** | M5ATOM + MATRIX macros. |

### Other

| `[env:*]` | Reference board | Build CI | HIL | Tag | Notes |
| --- | --- | --- | --- | --- | --- |
| `macchina-a0` | `esp32dev` (Macchina A0 automotive) | ✅ | — | **untested** | Flagged with `MACCHINA_A0`. Last meaningful community signal: discussion [#12](https://github.com/ESPresense/ESPresense/discussions/12). |

## Known hardware-specific issues

These are hardware-revision or peripheral-level breakages on a tier where the **base** firmware is in good shape — i.e. flagging them does **not** demote the whole tier.

| Variant | Issue | Status |
| --- | --- | --- |
| ESP32-C3-DevKitM-1-N4X (C3 v1.1 silicon, embedded flash) | [#2244](https://github.com/ESPresense/ESPresense/issues/2244) — boot loop after entry on all C3 firmware flavors (standard, CDC, verbose) since the Arduino v3 / IDF v5.4 migration in PR #2185. Root cause is firmware-size / partition-fit, tracked in [#2252](https://github.com/ESPresense/ESPresense/issues/2252). | **known-broken** on N4X. Other C3 boards on the HIL rig still pass. |
| ESP32-S3 (4.1.0b0) | [#2309](https://github.com/ESPresense/ESPresense/issues/2309) — board freeze. Triaged but reproducer is light. | **investigating** — not yet a known-broken designation. |
| ESP32 generic via web installer | [#2225](https://github.com/ESPresense/ESPresense/issues/2225) — manifest download failure. Likely environmental (CDN / corp proxy) rather than firmware. | **investigating** — distinct from any board variant. |

## Adding new boards

1. **Build only** is cheap: add an `[env:*]` to `platformio.ini` and a row here tagged `untested`. CI will keep the build green from then on.
2. **Community-tested** requires a confirmed working report on the pinned discussion: hardware revision, firmware version, date, name. The Community Manager updates the row.
3. **Tier-1** requires the board on the HIL rig. Coordinate with the Tech Lead — the rig is the bottleneck, not the YAML. `.woodpecker/hil.yml` only takes new boards as fast as the physical lab can host them.

## Gaps and follow-ups (QA backlog)

These are the things we know are missing and tracked separately, not silent omissions:

- **`esp32c6` HIL is `failure: ignore`** — either fix the failures or downgrade C6 from tier-1. Tech Lead's call ([ESPA-18](/ESPA/issues/ESPA-18)).
- **No HIL coverage for any `-cdc` variant** — at minimum a smoke test that the device enumerates as USB-CDC and accepts Improv over CDC instead of UART. Sub-task candidate.
- **No HIL coverage for M5Stack or Macchina** — these have no automated regression protection at all. If the project wants to keep shipping their `[env:*]` builds, we should either deprecate them or solicit a community-hosted HIL volunteer.
- **Real-RSSI fixtures** — distance-calc fixture suite ([PR #2317](https://github.com/ESPresense/ESPresense/pull/2317)) is currently synthetic seeds. Real captures from each tier-1 board would catch antenna/silicon variation that synthetic seeds miss.

## How to read this matrix when triaging a bug report

If a user reports a problem and their board is:

- **tier-1**: assume the firmware path is exercised every PR. The bug is most likely environment-specific (RF, mesh, neighbor, MQTT) or a hardware-revision break we haven't caught yet — request photos / chip markings.
- **tier-1 (soft-fail)** or **untested**: do not assume CI caught anything. Ask the user to test against the previous release before triaging as a regression.
- **known-broken**: link them to the existing issue and ask them to subscribe instead of opening a new one.
