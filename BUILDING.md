# Building

ESPresense is a plain [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) (v5.4) project.

```sh
git clone -b v5.4.4 --recursive https://github.com/espressif/esp-idf ~/esp/esp-idf
~/esp/esp-idf/install.sh esp32,esp32c3,esp32s3,esp32c6
. ~/esp/esp-idf/export.sh

./build.sh esp32                                  # build/esp32/espresense.bin
./build.sh esp32c3-cdc -p /dev/ttyACM0 flash monitor
```

`build.sh <env>` picks the chip target and sdkconfig fragments for a firmware variant
(`esp32`, `esp32c3`, `esp32c3-cdc`, `esp32c6`, `esp32s3`, `*-verbose`, `m5stickc`, `m5atom`,
`macchina-a0`, ...); `envs.cmake` maps the variant to compile definitions. Anything after the
variant name is passed to `idf.py`.

## Flashing and monitoring

Native-USB chips (C3/C6/S3) enumerate as `/dev/cu.usbmodem*`; external USB-UART bridges
(CP210x/CH34x) as `/dev/cu.usbserial*` or `/dev/ttyUSB*`.

```sh
./build.sh esp32c6 -p /dev/cu.usbmodem3101 flash monitor   # native USB-Serial/JTAG
./build.sh esp32 -p /dev/cu.usbserial-0001 flash monitor   # external USB-UART
```

The `-cdc` variants add `sdkconfig.cdc`, which routes the console over the native USB.

## Firmware size budget

Flash is 4 MB with a dual-OTA layout (`partitions_singleapp.csv`): `app0` and `app1` are each
`0x1E0000` (1920 KB), and each must hold a complete image. CI (`build.yml`) fails any variant
whose `espresense.bin` exceeds that, so read the byte-count line `idf.py` prints at the end of
a build when adding sensors or UI features.

## Web UI

```sh
cd ui
npm install
npm run build     # requires Node 22+, regenerates main/ui_*.h
```

`main/ui_*.h` are generated from `ui/` and committed, so a UI change must include the
regenerated headers. `ui-build.yml` normally opens a separate "Update UI build outputs" PR, but
it skips `dependabot/*` branches, so a Dependabot UI bump has to run `npm run build` itself.

## Releases

Release notes are drafted automatically by `release-drafter.yml` on every push to `main`, using
the PR labels from `.github/pr-labeler.yml` (branch prefixes `feature/*`/`feat/*`, `fix/*`,
`bugfix/*`, `bug/*`, `enhancement/*` map to labels). Version bumps come from PR labels:
`major`/`minor`/`patch`, defaulting to patch. Only label a PR if its branch prefix didn't
already set one.

Publishing is a tag push. `build.yml` builds all 15 variants, enforces the 1920 KB size gate,
and, on a `v*` tag, attaches every `<env>.bin` to the GitHub release via
`softprops/action-gh-release`. `FW_VERSION`/`FW_BRANCH` are baked into the firmware (see
`envs.cmake`): on a tag, `FW_VERSION` is the tag itself; on `main`/PRs it's the short SHA.

The draft release is marked `prerelease: true` with a `b` prerelease identifier, so a stable
release means editing the draft before publishing. Publishing a release (not the tag) triggers
`dispatch.yml`, which fires a `new-release` repository dispatch at `ESPresense/ESPresense.com`
to rebuild the docs site.

To cut a release: merge to `main`, let the drafter update the draft, edit the version/notes if
needed, then publish. `espresense.com/alpha/?pr=<n>` serves the PR build for testing before
merge.
