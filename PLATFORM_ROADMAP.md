# ESPresense platform-support roadmap — v5.0.0

This is a public stance, not a forecast. It tells you what we test, what we accept PRs for, and what we don't intend to chase. It will be revised as the project changes.

Tier definitions are below; the build system, per-chip tiers, and per-issue answers follow.

## Tiers

- **Tier 1 (flagship).** Builds, OTA, UI, and standard sensor wiring are all expected to work out of the box. Regressions block a release.
- **Tier 2 (supported, known caveats).** Builds are produced and shipped. Specific known issues are tracked publicly. Regressions don't block a release if the caveat is documented. No chip is in this tier as of v5.0.0 — it's here for the day one needs it, not because anything currently qualifies.
- **`help wanted`.** The build env exists or is in progress as a draft PR. The maintainers don't have the hardware (or the PR hasn't been rebased onto current `main`) to validate it. We will review and merge PRs that come with a reproduction or hardware reports.
- **Not on roadmap.** No builds, no plans. PRs that add it will be reviewed on the merits, but expect a high bar — they must not destabilize the supported targets.

## Build system

v5.0.0 drops `platformio.ini` entirely. The firmware is now a plain [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) (v5.4) project, built with `./build.sh <env>` — no PlatformIO toolchain, no Arduino layer.

Current envs, all built and size-checked in CI: `esp32`, `esp32-verbose`, `esp32c3`, `esp32c3-cdc`, `esp32c3-verbose`, `esp32c6`, `esp32c6-cdc`, `esp32c6-verbose`, `esp32s3`, `esp32s3-cdc`, `esp32s3-verbose`. `-cdc` routes the console over native USB instead of an external UART; `-verbose` turns on debug logging.

Hardware testing isn't uniform across that list, and that matters for what "tier 1" actually enforces: the four base envs (`esp32`, `esp32c3`, `esp32c6`, `esp32s3`) each get a hardware-in-the-loop (HIL) run on real boards on every PR, every push to `main`, and a nightly cron soak. The `-cdc` variants are compiled and size-checked in CI but never flashed to real hardware. `-verbose` variants only get HIL on a manual trigger — not on PRs, pushes, or cron — and even then, the manual `esp32c6-verbose` run is explicitly soft-fail (`failure: ignore`), unlike every other HIL step. "Tier 1" below is about the base envs; the PR-triggered HIL run on those is a required check on `main`, which is the actual release gate — the push and cron soak runs are regression-catchers, not something the release process reads.

v5 also drops the old board-specific PlatformIO environments for the M5Stick/M5Atom family (`m5stickc`, `m5stickc-plus`, `m5atom`) and for Macchina A0 — none of them exist as separate firmware builds anymore. M5Stick/M5Atom nodes still auto-update: CI publishes the plain `esp32` build under those old binary names too, so a node tracking (for example) `m5stickc.bin` updates onto `esp32` on its own. But the generic `esp32` build has no way to know which physical board it's on, so right after that update you need to apply that board's [template](https://espresense.com/configuration/templates) by hand to get LEDs, buttons, and I2C pins back — see [Upgrading from v4 to v5](https://espresense.com/upgrading-to-v5) for the exact steps per board. Macchina A0 is different: there's no `macchina-a0` binary alias, so a Macchina A0 node on v4 does **not** auto-update. Install the plain `esp32` build by hand via the [browser installer](https://espresense.com/firmware), then apply the [Macchina A0 template](https://espresense.com/nodes/macchina-a0). It's supported — just not over the air.

For board buying/compatibility advice (which board to get, what's flaky, what to avoid), see the pinned **[Boards — what works, what's flaky, what to avoid](https://github.com/ESPresense/ESPresense/discussions/2334)** discussion. This roadmap covers chip/build support only and intentionally doesn't duplicate that list.

## ESP32 chip variants

### ESP32 (original) — Tier 1

The flagship target, and still where most installs in the wild run. M5Stick/M5Atom boards build on top of this (see above).

### ESP32-S3 — Tier 1

Builds, HIL-tested on real hardware alongside the rest of the matrix. `-cdc` and `-verbose` variants build in the standard CI matrix.

### ESP32-C3 — Tier 1

[#2244](https://github.com/ESPresense/ESPresense/issues/2244) (boot loop on ESP32-C3-DevKitM-1-N4X after app entry) is closed and fixed, with a real root cause rather than a config workaround: C3 silicon revision v1.1 moved functions in ROM ([Espressif advisory AR2024-009](https://documentation.espressif.com/ar2024-009_en.html)), so firmware built on ESP-IDF older than 4.4.7 can't run on it — and on top of that, the web installer's static C3 bootloader predated that requirement and carried an invalid SHA-256 besides. The fix shipped in two places: the installer's C3 bootloader was rebuilt on ESP-IDF 5.4.4 ([ESPresense.com#411](https://github.com/ESPresense/ESPresense.com/pull/411)), and v5.0.0 builds C3 on IDF 5.4.4 directly; anyone staying on the v4 line gets a C3 build on IDF 4.4.7 as of [v4.0.8](https://github.com/ESPresense/ESPresense/releases/tag/v4.0.8). If you're on an affected board and haven't updated: install v4.0.8 or later, or v5.0.0, from [espresense.com/firmware](https://espresense.com/firmware/) and choose **Erase device**. C3 gets the same HIL treatment as every other tier-1 chip — real hardware, on every PR. If you hit a new C3 boot/connect problem, open a fresh issue with your chip revision (`esptool.py chip_id`); don't assume it's the old one.

### ESP32-C6 — Tier 1

The pure ESP-IDF rewrite added native C6 support directly — the earlier community PR attempting this ([#2042](https://github.com/ESPresense/ESPresense/pull/2042)) is closed, superseded by that rewrite, along with the C6 requests it was meant to answer ([#1270](https://github.com/ESPresense/ESPresense/issues/1270), [#1283](https://github.com/ESPresense/ESPresense/issues/1283), [#1354](https://github.com/ESPresense/ESPresense/issues/1354)). C6 runs in the same `hil.yml` pipeline as ESP32, C3, and S3, with no `failure: ignore` on its base env — the PR-triggered run of that pipeline is a required check on `main`, the same release gate every other tier-1 chip clears. That's the bar, and that's why it's the tier.

### ESP32-C2 / ESP32-H2 — Not on roadmap

Both chips have radios — this isn't a "missing hardware" story. **ESP32-C2** has Wi-Fi 4 and Bluetooth 5 (LE), but only 272 KB of SRAM and no native USB peripheral — noticeably less headroom than the ESP32-C3 we already ship as tier-1, for no offsetting benefit. **ESP32-H2** has Bluetooth LE 5.3 and 802.15.4 (Zigbee/Thread), but no Wi-Fi radio at all — ESPresense nodes talk MQTT over Wi-Fi, so H2 has no path to a broker without a separate gateway device. Adding either is out of scope. If your use case genuinely needs one — most likely H2 for an 802.15.4/Thread transport — open a discussion before a PR.

### ESP32-P4 — Not on roadmap (yet)

No Wi-Fi or Bluetooth radio on the SoC at all; it's built to pair with a separate network co-processor (for example a C6) over SPI/SDIO. Reconsider if we ever support that kind of companion-radio split. Not a near-term priority.

### New silicon in progress — `help wanted`, nothing shipped yet

None of the following are in a release. Don't plan around them yet.

- **ESP32-C5** — a draft PR ([#2450](https://github.com/ESPresense/ESPresense/pull/2450)) adds `esp32c5`/`esp32c5-cdc`/`esp32c5-verbose` envs and has been tested by its author on an ESP32-C5-DevKitC-1. Trying it is fine, but it isn't close to feasible yet: the firmware currently needs far more memory than the C5 has, and that needs major memory reduction work before C5 is viable — on top of an ESP-IDF bump. The PR itself notes C5 needs ESP-IDF ≥5.5, and current `main` is pinned to IDF v5.4.4; closing that gap needs the IDF v6.1 migration below (or an equivalent bump) merged first. It also predates the ESP-IDF rewrite above and still edits the now-deleted `platformio.ini`, so it needs a full rework onto `build.sh`/`envs.cmake` regardless. (See also [discussion #2501](https://github.com/ESPresense/ESPresense/discussions/2501) on C5 boards with an external antenna connector — same "no ESPresense build target yet" answer applies.) If you've got C5 hardware and want to help, start with memory reduction — #2450 stays open as the integration point once there's headroom.
- **ESP-IDF v6.1 migration** — a draft PR ([#2570](https://github.com/ESPresense/ESPresense/pull/2570)) moves every chip from IDF v5.4.4 to v6.1. It's groundwork, not chip-specific, and still a draft.
- **ESP32-S31** (Espressif's ESP-Mosaico board) — a draft PR ([#2571](https://github.com/ESPresense/ESPresense/pull/2571)) stacked on the IDF v6.1 migration above adds S31 support as an IDF 6 preview target. It can't land before that base migration does.

All three are drafts, not committed work. The IDF v6.1 migration and ESP32-S31 just need a clean rebase against `main` plus hardware to validate against; ESP32-C5 additionally needs major memory reduction, on top of that IDF bump, before it can even target current `main`. We'll fast-track review on each as soon as it clears that bar.

## Recurring quality complaints — public position

These are not promises of timelines. They are the position the project holds today.

### Wrong room despite stable XYZ — [#2220](https://github.com/ESPresense/ESPresense/issues/2220)

Canonical thread for "device coordinates look stable but the room assignment flips," most commonly in vertically-stacked rooms. The companion fix we originally pointed to for this never merged — it went stale and was closed unmerged without a rebase. Since then, companion's `multi_floor` locator strategy — which solves across every node in 3D and takes the floor from that result, instead of solving each floor separately and picking whichever floor scores best — has shipped, with several correctness fixes landed on top of it ([#1803](https://github.com/ESPresense/ESPresense-companion/pull/1803) and follow-ups). A brand-new install gets it automatically: companion writes out its bundled example config, with `multi_floor` already on, the first time it starts with no `config.yaml` on disk. An existing install keeps whatever `config.yaml` it already has, so if you deployed before this changed, you need to opt in by hand.

If you're hitting this: update companion to [v2.6.3](https://github.com/ESPresense/ESPresense-companion/releases/tag/v2.6.3) or later. On a new install, `multi_floor` is already on — nothing to do. On an existing install, open your `config.yaml` and set `locators.multi_floor.enabled: true`, then turn off your per-floor locator (for example `locators.nadaraya_watson.enabled: false`). Report back on [#2220](https://github.com/ESPresense/ESPresense/issues/2220) whether that fixes it for you — we haven't re-verified it against the original report ourselves, so treat this as "likely fixed on a new install, opt-in on an existing one," not "closed."

### ESP32-C3 boot loop

Closed — see the [C3 tier section](#esp32-c3--tier-1) above for the root cause and fix.

### "Failed to download manifest" in web flasher

Closed. The manifest route used to list release assets through the live, unauthenticated GitHub API, and failed whenever that call was rate-limited or GitHub had a hiccup — that's [#2225](https://github.com/ESPresense/ESPresense/issues/2225). The actual fix ([ESPresense.com#385](https://github.com/ESPresense/ESPresense.com/pull/385)) stopped depending on the GitHub API at request time and added a cache layer that serves the last good manifest if a lookup fails. [ESPresense.com#355](https://github.com/ESPresense/ESPresense.com/pull/355) added a probe that watches those endpoints going forward — that's monitoring for a regression, not the fix itself.

## What this means for contributors

- If you have an open feature request against C2 / H2 / P4 — we will close as `not planned` with a pointer to this discussion. No prejudice; the policy is just clear now.
- If you want to help land the ESP-IDF v6.1 migration or ESP32-S31 support — the open draft PRs above are the place to start. Rebase onto current `main`, bring hardware to validate against. ESP32-C5 needs memory reduction first, on top of the IDF bump, before the same treatment applies.
- If you're debugging an issue on a `help wanted` target, please attach the bootlog, chip variant, flash size, and which build env you used.

This roadmap will be revised as the targets shift.
