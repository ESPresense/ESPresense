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

Pick the serial port for your board. Native-USB chips (C3/C6/S3, and any board wired for the
built-in USB-Serial/JTAG) enumerate as `/dev/cu.usbmodem*`; boards with an external USB-UART
bridge (CP210x/CH34x) show up as `/dev/cu.usbserial*` or `/dev/ttyUSB*`.

```sh
./build.sh esp32c6 -p /dev/cu.usbmodem3101 flash monitor   # native USB-Serial/JTAG
./build.sh esp32c3-cdc -p /dev/ttyACM0 flash monitor       # native USB (CDC variant)
./build.sh esp32 -p /dev/cu.usbserial-0001 flash monitor   # external USB-UART
```

The `-cdc` variants (`esp32c3-cdc`, `esp32c6-cdc`, `esp32s3-cdc`) add `sdkconfig.cdc`, which
routes the console over the native USB. Use them for boards whose UART is not broken out.

## Firmware size budget

Flash is 4 MB with a **dual-OTA** layout (`partitions_singleapp.csv`): `app0` and `app1` are
each **`0x1E0000` = 1,966,080 bytes (1920 KB)**, and each slot must hold a complete image.

`idf.py` prints the headroom at the end of every build:

```
espresense.bin binary size 0x157620 bytes. Smallest app partition is 0x1e0000 bytes. 0x889e0 bytes (28%) free.
```

CI (`build.yml`) fails any variant whose `espresense.bin` exceeds 1920 KB, so an oversized
image can't reach a release — but keep an eye on it when adding sensors or UI features. The
largest variants are `esp32` (Ethernet + all sensors) and `esp32c6`; the web UI is a few tens
of KB compressed and not usually the binding constraint.

## Web UI

```sh
cd ui
npm install
npm run build     # regenerates main/ui_*.h
```

Requires **Node 22** or newer (matches the UI workflows, `ui-build.yml` / `ui-tests.yml`). The `main/ui_*.h` headers are generated
from `ui/` and committed, so a UI change is not complete until the rebuild headers are
included. The `ui-build.yml` workflow opens a separate "Update UI build outputs" PR when
`ui/**` changes, but it **skips `dependabot/*` branches** — for a Dependabot UI bump you must
run `npm run build` and commit the regenerated headers in the bump PR itself.

Host unit tests: `./test/run.sh`.

Hardware-in-the-loop and soak testing are described in `AGENTS.md` (`.woodpecker/hil.yml`,
`soak/<name>` branches, `[soak]` in the PR title).
