# ESP32-C6 Compatibility Matrix (initial audit)

Date: 2026-02-16
Scope: repo-level support snapshot for bead `espresense-5` consolidation (#1270 #1283 #1354 #1303)

## Current target matrix

- **ESP32 (`env:esp32`)**: ✅ explicit build target
- **ESP32-C3 (`env:esp32c3`, `env:esp32c3-cdc`)**: ✅ explicit build targets
- **ESP32-S3 (`env:esp32s3`, `env:esp32s3-cdc`)**: ✅ explicit build targets
- **ESP32-C6**: ❌ no explicit build target

## C6 support gaps found

1. **No PlatformIO environment**
   - `platformio.ini` has no `[env:esp32c6]` (or cdc/verbose variants).

2. **No C6 compile-time defines wired**
   - No `CONFIG_IDF_TARGET_ESP32C6`, `ARDUINO_ARCH_ESP32C6`, or `ESP32C6` build flags configured in `platformio.ini`.

3. **Board-default handling is incomplete for C6**
   - `include/defaults.h` has chip-specific branches for `ESP32C3` and `ESP32S3`, but none for `ESP32C6`.
   - C6 currently falls through generic defaults (likely incorrect for many C6 dev boards, especially I2C pins/calibration assumptions).

## Practical implication

C6 is currently **not a first-class supported target** in this tree. Reports across #1270/#1283/#1354/#1303 are likely symptoms of this missing target wiring plus defaults drift.

## Suggested next implementation step

Add a minimal `[env:esp32c6]` (and optional `-cdc`) in `platformio.ini` derived from C3 settings, with C6-specific build flags and board selection, then run a compile smoke test for that env.
