#!/usr/bin/env bash
# Merge one built variant into a single image to write at offset 0x0:
#   ./tools/merge-factory.sh esp32c3 -> build/esp32c3/espresense.factory.bin
# Bootloader + partition table + initial OTA data + app, exactly as `idf.py flash` would write them
# (build/<env>/flash_args), for serial flashing in one step and for full-image OTA installers.
set -euo pipefail
ENV="${1:?usage: merge-factory.sh <env>}"
DIR="build/$ENV"
TARGET=$(sed -n 's/^CONFIG_IDF_TARGET="\(.*\)"$/\1/p' "$DIR/sdkconfig")
cd "$DIR"
# flash_args lists every image with its offset and the flash mode/freq/size the bootloader header needs.
esptool.py --chip "$TARGET" merge_bin -o espresense.factory.bin @flash_args
