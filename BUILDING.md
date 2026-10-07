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
