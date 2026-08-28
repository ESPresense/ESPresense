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

Host unit tests: `./test/run.sh`. Web UI: `cd ui && npm run build` regenerates `main/ui_*.h`.
